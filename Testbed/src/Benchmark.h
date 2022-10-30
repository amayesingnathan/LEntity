#pragma once

#include "ECS/Registry.h"

namespace LEnt {

	struct BenchmarkData
	{
		std::string name;
		Timepoint start = Clock::now();

		BenchmarkData() = delete;
		BenchmarkData(const std::string& n) : name(n) {}
	};

	struct BenchmarkResult
	{
		std::string name;
		usize timing = 0;

		friend std::ostream& operator<<(std::ostream& out, const BenchmarkResult& result);
	};

	class Benchmark
	{
	public:
		Benchmark(const std::string& filename = "results.json");

		void run(usize entityCount);

	private:
		void Init();
		void End();

		void RunInternal(usize entityCount);

		void CreateEntities();
		void AddComponent();
		void RemoveComponent();
		void AddComponents();
		void RemoveComponents();
		void UpdateComponent();
		void UpdateComponents();

	private:
		inline void BeginTask(const std::string& taskName);
		inline void EndTask();

	private:
		std::ofstream mFileStream;
		Registry mRegistry;
		std::vector<EntityID> mEntities;

		BenchmarkData* mCurrentTask = nullptr;
		std::unordered_map<std::string, usize> mTimings;
	};

}