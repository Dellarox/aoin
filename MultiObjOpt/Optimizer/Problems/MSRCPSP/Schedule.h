#pragma once

#include "Task.h"

class CSchedule
{
public:

	const std::vector<CTask>& GetTasks() const { return m_Tasks; };
	const std::vector<CResource>& GetResources() const { return m_Resources; };

	void SetTasks(const std::vector<CTask>& tasks);
	void SetResources(const std::vector<CResource>& resources);

	void Assign(size_t taskIndex, TResourceID resourceId);
	void BuildTimestamps();

	void BuildTimestampForTask(CTask& task);

	void Clear();
	void Reset();

	float EvaluateDuration();
	float EvaluateCost();
	float EvaluateAvgCashFlowDev();
	float EvaluateSkillOveruse();
	float EvaluateAvgUseOfResTime();
	void GetCapableResources(const CTask& task, std::vector<TResourceID>& resourceIds) const;
	TTime GetEarliestTime(const CTask& task) const;

	TTime GetMaxDuration() const;
	TTime GetMinDuration() const;
	float GetMaxCost() const;
	float GetMinCost() const;
	float GetMaxAvgCashFlowDev() const;
	float GetMinAvgCashFlowDev() const;
	float GetMaxSkillOveruse() const;
	float GetMinSkillOveruse() const;
	float GetMaxAvgUseOfResTime() const;
	float GetMinAvgUseOfResTime() const;

private:
	std::vector<CTask> m_Tasks;
	std::vector<CResource> m_Resources;
};
