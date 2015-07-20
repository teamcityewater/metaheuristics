#pragma once

#include <string>
#include <vector>
#include <map>

using namespace std;

namespace mhcpp
{
	class ISystemConfiguration
	{
	public:
		virtual ~ISystemConfiguration() {}
		/// <summary>
		/// Gets an alphanumeric description for this system configuration
		/// </summary>
		virtual string GetConfigurationDescription() = 0;

		/// <summary>
		/// Apply this system configuration to a compatible system, usually a 'model' in the broad sense of the term.
		/// </summary>
		/// <param name="system">A compatible system, usually a 'model' in the broad sense of the term</param>
		/// <exception cref="ArgumentException">thrown if this system configuration cannot be meaningfully applied to the specified system</exception>
		virtual void ApplyConfiguration(void* system) = 0;
	};

	/// <summary>
	/// Interface for system configurations that support cloning. Helps to support parallelism in solvers.
	/// </summary>
	class ICloneableSystemConfiguration : ISystemConfiguration //, ICloningSupport<ICloneableSystemConfiguration>
	{
	public:
		virtual ~ICloneableSystemConfiguration() {}
	};

	/// <summary>
	/// Interface for system configurations that are a set of numeric parameters, each with min and max feasible values.
	/// </summary>
	/// <typeparam name="T">A comparable type; typically a float or double, but possibly integer or more esoteric type</typeparam>
	template<typename T>
	class IHyperCube : ICloneableSystemConfiguration //where T : IComparable
	{
	public:
		virtual ~IHyperCube() {}
		/// <summary>
		/// Gets the names of the variables defined for this hypercube.
		/// </summary>
		/// <returns></returns>
		virtual vector<string> GetVariableNames() = 0;

		/// <summary>
		/// Gets the number of dimensions in this hypercube
		/// </summary>
		virtual size_t Dimensions() = 0;

		/// <summary>
		/// Gets the value for a variable
		/// </summary>
		/// <param name="variableName"></param>
		/// <returns></returns>
		virtual T GetValue(string variableName) = 0;

		/// <summary>
		/// Gets the maximum feasible value for a variable
		/// </summary>
		/// <param name="variableName"></param>
		/// <returns></returns>
		virtual T GetMaxValue(string variableName) = 0;

		/// <summary>
		/// Gets the minimum feasible value for a variable
		/// </summary>
		/// <param name="variableName"></param>
		/// <returns></returns>
		virtual T GetMinValue(string variableName) = 0;

		/// <summary>
		/// Sets the value of one of the variables in the hypercube
		/// </summary>
		/// <param name="variableName"></param>
		/// <param name="value"></param>
		virtual void SetValue(string variableName, T value) = 0;

		/// <summary>
		/// Perform a homotetic transformation centered around this hypercube, of a second hypercube.
		/// </summary>
		/// <param name="point">The hypercube from which to derive. This object must not be modified by the method.</param>
		/// <param name="factor">The factor in the homotecie. a value 1 leaves the effectively point unchanged</param>
		/// <returns>A new instance of an hypercube, result of the transformation</returns>
		//virtual IHyperCube<T> HomotheticTransform(IHyperCube<T> point, double factor) = 0;
	};

	template<typename T>
	class IHyperCubeSetBounds : IHyperCube < T > //where T : IComparable
	{
	public:
		virtual ~IHyperCubeSetBounds() {}
		virtual void SetMinValue(string variableName, T value) = 0;
		virtual void SetMaxValue(string variableName, T value) = 0;
		virtual void SetMinMaxValue(string variableName, T min, T max, T value) = 0;
	};

	class IObjectiveScore
	{
	public:
		virtual ~IObjectiveScore() {}
		/// <summary>
		/// Gets whether this objective is a maximizable one (higher is better).
		/// </summary>
		virtual bool Maximise();

		/// <summary>
		/// Get a text represtattion of this score
		/// </summary>
		/// <returns></returns>
		virtual string GetText() = 0;

		/// <summary>
		/// Get name of the objective measure, typically a bivariate statistic.
		/// </summary>
		virtual string Name() = 0;

		//	/// <summary>
		//	/// Gets the value of the objective. Inheritors should return the real value, and not worry about negating or not. This is taken care elsewhere.
		//	/// </summary>
		//IComparable ValueComparable() = 0;
	};

	/// <summary>
	/// An interface for one or more objective scores derived from the evaluation of a candidate system configuration.
	/// </summary>
	/// <remarks>This interface is defined without generics on purpose, to reduce complexity. Limits the unnecessary proliferation of generic classes</remarks>
	class IBaseObjectiveScores
	{
	public:
		virtual ~IBaseObjectiveScores() {}
		/// <summary>
		/// Gets the number of objective scores in this instance.
		/// </summary>
		virtual size_t ObjectiveCount() = 0;

		/// <summary>
		/// Gets one of the objective 
		/// </summary>
		/// <param name="i">zero-based inex of the objective</param>
		/// <returns></returns>
		virtual IObjectiveScore * GetObjective(int i) = 0;

		/// <summary>
		/// Gets the system configuration that led to these scores.
		/// </summary>
		/// <returns></returns>
		virtual ISystemConfiguration * GetSystemConfiguration() = 0;
	};

	/// <summary>
	/// A generic interface for one or more objective scores derived from the evaluation of a candidate system configuration.
	/// </summary>
	/// <typeparam name="TSysConf">The type of the system configuration</typeparam>
	template<typename TSysConf>
	class IObjectiveScores : IBaseObjectiveScores //where TSysConf : ISystemConfiguration
	{
	public:
		virtual ~IObjectiveScores() {}
		/// <summary>
		/// Gets the system configuration that led to these scores.
		/// </summary>
		virtual TSysConf SystemConfiguration() = 0;
	};

	template<typename T>
	class ICandidateFactory
	{
	};

	template<typename T>
	class IEvolutionEngine
	{
	};

	template<typename T>
	class ITerminationCondition
	{
	public:
		void SetEvolutionEngine(IEvolutionEngine<T>* engine){};
	};

	template<typename T>
	class IFitnessAssignment
	{
	};

	class IRandomNumberGeneratorFactory
	{
	};

	template<typename TSysConf>
	class IObjectiveEvaluator
	{
	public:
		virtual ~IObjectiveEvaluator() {}
		/// <summary>
		/// Evaluate the objective values for a candidate system configuration
		/// </summary>
		/// <param name="systemConfiguration">candidate system configuration</param>
		/// <returns>An object with one or more objective scores</returns>
		virtual IObjectiveScores<TSysConf> EvaluateScore(TSysConf systemConfiguration) = 0;
		virtual bool IsCloneable() { return false; }
	};

	/// <summary>
	/// A superset of the <see cref="IObjectiveEvaluator"/> interface that is clonable, most notably to spawn evaluators that are thread safe.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <remarks>
	/// This interface is used for instance by the parallel version of the shuffled complex evolution algorithm, 
	/// when spawning complexes that will be run in parallel and for which we want thread safe structures, 
	/// e.g. not sharing the same model runner.
	/// </remarks>
	//template<typename TSysConf>
	//class IClonableObjectiveEvaluator<TSysConf> :
	//	IObjectiveEvaluator<TSysConf> //, ICloningSupport<IClonableObjectiveEvaluator<TSysConf>>
	//	//		where TSysConf : ISystemConfiguration
	//{
	//};

	///// <summary>
	///// Interface for scores used to quantify the performance of a system, defining the type of the score.
	///// </summary>
	///// <typeparam name="T">The type of the objective (score) value, e.g. double, int, float</typeparam>
	//template<typename T>
	//class IObjectiveScore //<out T> where T : IComparable
	//{
	//public:
	//	virtual ~Class() {}
	//	/// <summary>
	//	/// Gets the value of the objective.
	//	/// </summary>
	//	virtual T Value() = 0;
	//};


	/// <summary>
	/// An interface for population based search algorithms
	/// </summary>
	/// <typeparam name="V">The type of the fitness score used to evolve the algorithm</typeparam>
	//template<typename V>
	//class IPopulation
	//	//		where V : IComparable
	//{
	//public:
	//	virtual ~IPopulation() {}
	//	FitnessAssignedScores<V>[] Population() = 0;
	//};


	/// <summary>
	/// An interface for constructs where the optimization problem is given to a solver, and ready to execute.
	/// </summary>
	//template<typename TSysConf>
	//class IEvolutionEngine
	//{
	//public:
	//	virtual ~IEvolutionEngine() {}
	//	/// TODO: think of ways to monitor the execution.

	//	/// <summary>
	//	/// Solve the metaheuristic this object defines.
	//	/// </summary>
	//	/// <returns>The results of the optimization process</returns>
	//	virtual IOptimizationResults<TSysConf> Evolve() = 0;

	//	/// <summary>
	//	/// Gets a description of this solver
	//	/// </summary>
	//	/// <returns></returns>
	//	virtual string GetDescription() = 0;

	//	/// <summary>
	//	/// Request a cancellation of the process.
	//	/// </summary>
	//	virtual void Cancel() = 0;

	//};


	template<typename T>
	class HyperCube : IHyperCube<T> //where T : IComparable
	{
	public:
		vector<string> GetVariableNames() {
			vector<string> res;
			return res;
		}
		void Define(string name, double min, double max, double value) {
			def[name] = MMV(name, min, max, value);
		}
		size_t Dimensions() { return def.size(); }
		T GetValue(string variableName) { return def[variableName].Value; }
		T GetMaxValue(string variableName) { return def[variableName].Max; }
		T GetMinValue(string variableName) { return def[variableName].Min; }
		void SetValue(string variableName, T value)    { def[variableName].Value = value; }
		void SetMinValue(string variableName, T value) { def[variableName].Min = value; }
		void SetMaxValue(string variableName, T value) { def[variableName].Max = value; }
		//IHyperCube<T> HomotheticTransform(IHyperCube<T> point, double factor) {}
		string GetConfigurationDescription() { return ""; }
		void ApplyConfiguration(void* system) {}

	private:
		class MMV
		{
		public:
			MMV(){}
			MMV(string name, double min, double max, double value) : 
				Name(name), Max(max), Min(min), Value(value)
			{
			}
			string Name;
			double Min, Max, Value;
		};
		std::map<string, MMV> def;
	};

	/// <summary>
	/// Capture a fitness score derived from a candidate system configuration and its objective scores.
	/// </summary>
	/// <typeparam name="T">The type of fitness used to compare system configuration.</typeparam>
	template<typename T, typename TSys>
	class FitnessAssignedScores //: IComparable<FitnessAssignedScores<T>> where T : IComparable
	{
		/// <summary>
		/// Creates a FitnessAssignedScores, a union of a candidate system configuration and its objective scores, and an overall fitness score.
		/// </summary>
		/// <param name="scores">Objective scores</param>
		/// <param name="fitnessValue">Fitness value, derived from the scores and context information such as a candidate population.</param>
		FitnessAssignedScores(IObjectiveScores<TSys>* scores, T fitnessValue)
		{
			this->Scores = scores;
			this->FitnessValue = fitnessValue;
		}
		/// <summary>
		/// Gets the objective scores
		/// </summary>
		IObjectiveScores<TSys>* Scores;

		/// <summary>
		/// Gets the fitness value that has been assigned to the candidate system configuration and its objective scores
		/// </summary>
		T FitnessValue;

		/// <summary>
		/// Compares two FitnessAssignedScores<T>.
		/// </summary>
		/// <param name="other">Object to compare with this object</param>
		/// <returns>an integer as necessary to implement IComparable</returns>
		int CompareTo(FitnessAssignedScores<T, TSys> other)
		{
			return this->FitnessValue.CompareTo(other.FitnessValue);
		}

		string ToString()
		{
			return FitnessValue.ToString() + ", " + Scores.ToString();
		}
	};

	class IHyperCubeOperations
	{
		virtual IHyperCube<double> GetCentroid(std::vector<IHyperCube<double>> points) = 0;
		virtual IHyperCube<double> GenerateRandomWithinHypercube(std::vector<IHyperCube<double>> points) = 0;
		virtual IHyperCube<double> GenerateRandom(IHyperCube<double> point) = 0;
	};

}