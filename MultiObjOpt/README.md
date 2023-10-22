### MetaheuristicsRepo

The main program within this repository is engineered to optimize planning and scheduling problems using meta-
heuristic methods. Users can designate the problem type and its definition and configure the optimization method.
All results from the optimization will be saved to a specified output director

## Usage

### Command Line Arguments:

The program anticipates the following command-line arguments, presented in the specified sequence:

1. Optimizer configuration path.
2. Problem name.
3. Problem definition path.
4. Output directory path.
5. (Optional) Number of executions (default is 1).
6. (Optional) Silent mode (1 for silent, 0 for verbose; default is verbose).

### Supported Problem Names:

- MSRCPSP_TaskAssign
- TTP CityOrderKnapBin_MO
- TTP CityOrderKnapBin_SO

### Example:
./MetaheuristicsRepo ../MultiObjOpt/OptimizersConfigurations/GeneticAlgorithm/TTP_SO/GA_TTP_SO.cfg  TTP_CityOrderKnapBin_SO  ../MultiObjOpt/ProblemsDefinitions/TTP/berlin52_n51_bounded-strongly-corr_01.ttp  ../output

### Notes:

**For Visual Studio users:**

1. Right-click on the project in the Solution Explorer.
2. Select ’Properties’ from the context menu.
3. In the opened properties window, navigate to Configuration Properties -> Debugging.
4. Find the ’Command Arguments’ field.
5. Enter your command-line arguments in the ’Command Arguments’ field.
6. Click ’OK’ to save changes and close the properties window.

**For CLion users:**

1. Click on the ’Edit Configuration’ button at the top right corner of the IDE window.
2. In the opened configuration window, find the ’Program arguments’ field.
3. Enter your command-line arguments in the ’Program arguments’ field.
4. Click ’OK’ to save changes and close the configuration window.

## Additional Information

- **Single-objective GA**:
    - **TTP**:
        - If you provided the ObjectiveWeights parameter, use TTP CityOrderKnapBin MO problem name.
        - If you did not provide the ObjectiveWeights parameter, use TTP CityOrderKnapBin SO problem name.
        - Use TTP specific operators TTP OX SX and Mutation TTP Reverse Flip.
    - **MSRCPSP**:
        - The ObjectiveWeights parameter must be provided.
        - Use TTP specific operators UniformCX and RandomBit.

- **NSGAII**:
    - Do not provide the ObjectiveWeights parameter.
    - Use operators specific to a problem.

- **Directories**:
    - Configurations: Example optimizer configurations can be found in the `OptimizersConfigurations` directory.
    - Definitions: Problem definitions are in the `ProblemsDefinitions` directory.
    - Ensure that paths for the optimizer configuration, problem definition, and output directory are accurate and have the necessary permissions.

- **Error Handling**: Incorrect problem or optimizer name inputs will result in an error message from the program.
- **Summary**: The software summarizes the results in the designated output directory.
- **Experimentation**: It's possible to experiment with other optimizers and problems, but successful execution is not guaranteed.
