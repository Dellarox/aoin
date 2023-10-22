#include "Schedule.h"

void CSchedule::SetTasks(const std::vector<CTask>& tasks)
{
	m_Tasks = tasks;
}

void CSchedule::SetResources(const std::vector<CResource>& resources)
{
	m_Resources = resources;
}

void CSchedule::Assign(size_t taskIndex, TResourceID resourceId)
{
	m_Tasks[taskIndex].SetResourceID(resourceId);
}

void CSchedule::BuildTimestamps()
{
	for (CResource& resource : m_Resources)
	{
		resource.SetFinish(0);
		resource.SetWorkingTime(0);
	}

	// Reset tasks start
	for (size_t i = 0; i < m_Tasks.size(); ++i)
	{
		m_Tasks[i].SetStart(-1);
	}

	// Assign tasks with relation requirements
	for (size_t i = 0; i < m_Tasks.size(); ++i)
	{
		if (m_Tasks[i].GetHasSuccessors())
		{
			BuildTimestampForTask(m_Tasks[i]);
		}
	}

	// Assign rest of the tasks
	for (size_t i = 0; i < m_Tasks.size(); ++i)
	{
		if (!m_Tasks[i].GetHasSuccessors())
		{
			BuildTimestampForTask(m_Tasks[i]);
		}
	}
}

void CSchedule::BuildTimestampForTask(CTask& task)
{
	CResource& resource = m_Resources[(size_t)task.GetResourceID() - 1];
	TTime start = std::max(GetEarliestTime(task), resource.GetFinish());
	task.SetStart(start);
	resource.SetFinish(start + task.GetDuration());
	resource.AddWorkingTime(task.GetDuration());
}

void CSchedule::Clear()
{
	m_Tasks.clear();
	m_Resources.clear();
}

void CSchedule::Reset()
{
	for (CTask& task : m_Tasks)
	{
		task.SetStart(-1);
		task.SetResourceID(-1);
	}

	for (CResource& resource : m_Resources)
	{
		resource.SetFinish(-1);
	}
}

float CSchedule::EvaluateDuration()
{
	TTime duration = 0;
	for (const CResource& resource : m_Resources)
	{
		if (resource.GetFinish() > duration)
		{
			duration = resource.GetFinish();
		}
	}
	return duration;
}

float CSchedule::EvaluateCost()
{
	float cost = 0.f;
	for (const CTask& task : m_Tasks)
	{
		TResourceID resourceId = task.GetResourceID();
		if (resourceId > 0)
		{
			CResource& resource = m_Resources[(size_t)task.GetResourceID() - 1];
			cost += resource.GetSalary() * task.GetDuration();
		}
	}
	return cost;
}

float CSchedule::EvaluateAvgCashFlowDev()
{
	// Calculate total duration
	TTime duration = 0;
	for (const CResource& resource : m_Resources)
	{
		if (resource.GetFinish() > duration)
		{
			duration = resource.GetFinish();
		}
	}

	// Init time stamps with 0
	std::vector<float> cashFlows((size_t)duration, 0.f);
	float totalCashFlow = 0.f;
	for (const CTask& task : m_Tasks)
	{
		if (task.GetStart() >= 0)
		{
			TResourceID resourceId = task.GetResourceID();
			if (resourceId > 0)
			{
				CResource& resource = m_Resources[(size_t)task.GetResourceID() - 1];
				float resourceSalary = resource.GetSalary();
				for (TTime i = task.GetStart(); i < task.GetExpectedFinish(); ++i)
				{
					cashFlows[i] += resourceSalary;
					totalCashFlow += resourceSalary;
				}
			}
		}
	}

	float avgCashFlow = totalCashFlow / (float)duration;

	// Calculate deviation
	float totalCashFlowDeviation = 0.f;
	for (const float& cashFlow : cashFlows)
	{
		totalCashFlowDeviation += fabsf(cashFlow - avgCashFlow);
	}
	return totalCashFlowDeviation;
}

float CSchedule::EvaluateSkillOveruse()
{
	// In my opinion - it could be calculated differently
	// Now, it does not include resource's additional skills which are not required by a task
	float overuse = 0.f;
	for (const CTask& task : m_Tasks)
	{
		TResourceID resourceId = task.GetResourceID();
		if (resourceId > 0)
		{
			CResource& resource = m_Resources[(size_t)task.GetResourceID() - 1];
			for (const SSkill& reqSkill : task.GetRequiredSkills())
			{
				TSkillLevel resSkillLevel = 0;
				if (resource.GetSkillLevel(reqSkill.m_TypeID, resSkillLevel))
				{
					overuse += (resSkillLevel - reqSkill.m_Level);
				}
			}
		}
	}
	return overuse;
}

float CSchedule::EvaluateAvgUseOfResTime()
{
	// It's more of a deviation from expected work time
	// TODO - store it somewhere to avoid using static inside the function
	static const float s_ExpectedUsedTime = (float)GetMaxDuration() / (float)m_Resources.size();
	float workingTimeSum = 0.f;
	for (const CResource& res : m_Resources)
	{
		workingTimeSum += fabsf(res.GetWorkingTime() - s_ExpectedUsedTime);
	}
	return workingTimeSum;
}

void CSchedule::GetCapableResources(const CTask& task, std::vector<TResourceID>& resourceIds) const
{
	resourceIds.clear();
	for (const CResource& resource : m_Resources)
	{
		if (task.CanBeExecutedBy(resource))
		{
			resourceIds.push_back(resource.GetResourceID());
		}
	}
}

TTime CSchedule::GetEarliestTime(const CTask& task) const
{
	TTime earliestTime = 0;

	for (TTaskID predId : task.GetPredecessors())
	{
		TTime expectedFinish = m_Tasks[(size_t)predId - 1].GetExpectedFinish();
		if (expectedFinish > earliestTime)
		{
			earliestTime = expectedFinish;
		}
	}

	return earliestTime;
}

TTime CSchedule::GetMaxDuration() const
{
	TTime duration = 0;
	for (const CTask& task : m_Tasks)
	{
		duration += task.GetDuration();
	}
	return duration;
}

TTime CSchedule::GetMinDuration() const
{
	TTime minDuration = TIME_MAX;
	for (const CTask& task : m_Tasks)
	{
		const TTime& taskDuration = task.GetDuration();
		if (taskDuration < minDuration)
		{
			minDuration = taskDuration;
		}
	}

	return (minDuration * (TTime)m_Tasks.size()) / (TTime)m_Resources.size();
}

float CSchedule::GetMaxCost() const
{
	float mostExpRes = 0.f;
	for (const CResource& resource : m_Resources)
	{
		mostExpRes = fmaxf(mostExpRes, resource.GetSalary());
	}
	float maxCost = 0.f;
	for (const CTask& task : m_Tasks)
	{
		maxCost += (task.GetDuration() * mostExpRes);
	}
	return maxCost;
}

float CSchedule::GetMinCost() const
{
	float minExpRes = FLT_MAX;
	for (const CResource& resource : m_Resources)
	{
		minExpRes = fminf(minExpRes, resource.GetSalary());
	}
	float minCost = 0.f;
	for (const CTask& task : m_Tasks)
	{
		minCost += (task.GetDuration() * minExpRes);
	}
	return minCost;
}

float CSchedule::GetMaxAvgCashFlowDev() const
{
	return GetMaxCost();
}

float CSchedule::GetMinAvgCashFlowDev() const
{
	return 0.f;
}

float CSchedule::GetMaxSkillOveruse() const
{
	float maxOveruse = 0.f;
	for (const CTask& task : m_Tasks)
	{
		for (const SSkill& reqSkill : task.GetRequiredSkills())
		{
			TSkillLevel maxSkillLevel = 0;
			for (const CResource& resource : m_Resources)
			{
				TSkillLevel resSkillLevel = 0;
				if (resource.GetSkillLevel(reqSkill.m_TypeID, resSkillLevel) && resSkillLevel > maxSkillLevel)
				{
					maxSkillLevel = resSkillLevel;
				}
			}
			maxOveruse += (maxSkillLevel - reqSkill.m_Level);
		}
	}
	return maxOveruse;
}

float CSchedule::GetMinSkillOveruse() const
{
	return 0.f;
}

float CSchedule::GetMaxAvgUseOfResTime() const
{
	// TODO - store it somewhere to avoid using static inside the function
	static const float s_ExpectedUsedTime = (float)GetMaxDuration() / (float)m_Resources.size();
	return s_ExpectedUsedTime * (m_Resources.size() - 1);
}

float CSchedule::GetMinAvgUseOfResTime() const
{
	return 0.f;
}
