#include "Benchmark.h"

namespace LEnt {

	static constexpr usize sIterations = 256;

	struct Position
	{
		f32 pos[3] = { 0 };
	};
	struct Velocity
	{
		f32 vel[3] = { 0 };
	};

	std::ostream& operator<<(std::ostream& out, const BenchmarkResult& result)
	{
		out << "\n";
		out << "\t\t{\n";
		out << "\t\t\t\"Name\": \"" << result.name << "\"\n";
		out << "\t\t\t\"Duration\": " << result.timing << "\n";
		out << "\t\t},";

		out.flush();
		return out;
	}

	Benchmark::Benchmark(const std::string& filename)
		: mFileStream(filename, std::ios::binary)
	{
	}

	void Benchmark::run(size_t entityCount)
	{
		LE_PROFILE_FUNCTION();

		Init();

		for (usize i = 0; i < sIterations; i++)
			RunInternal(entityCount);

		for (const auto& [name, timing] : mTimings)
			mFileStream << BenchmarkResult{ name, timing / sIterations };

		End();
	}

	void Benchmark::Init()
	{
		LE_ASSERT(mFileStream.is_open(), "Could not open output file!");
		mFileStream.clear();
		mFileStream << "{\n\t[";
	}

	void Benchmark::End()
	{
		mFileStream.seekp(-1, std::ios_base::end);
		mFileStream << "\n\t]\n}";
	}

	void Benchmark::RunInternal(usize entityCount)
	{
		LE_PROFILE_FUNCTION();

		mRegistry.clear();
		mEntities = std::vector<EntityID>(entityCount);

		CreateEntities();

		AddComponent();
		RemoveComponent();

		AddComponents();
		RemoveComponents();
	}

	void Benchmark::CreateEntities()
	{
		BeginTask("CreateEntities");

		for (usize i = 0; i < mEntities.size(); i++)
			mEntities[i] = mRegistry.create();

		EndTask();
	}

	void Benchmark::AddComponent()
	{
		BeginTask("AddComponent");

		for (EntityID entity : mEntities)
			mRegistry.erase<Position>(entity);

		EndTask();
	}

	void Benchmark::RemoveComponent()
	{
		BeginTask("RemoveComponent");

		for (EntityID entity : mEntities)
			mRegistry.erase<Position>(entity);

		EndTask();
	}

	void Benchmark::AddComponents()
	{
		BeginTask("AddComponents");

		for (EntityID entity : mEntities)
		{
			mRegistry.emplace<Position>(entity);
			mRegistry.emplace<Velocity>(entity);
		}

		EndTask();
	}

	void Benchmark::RemoveComponents()
	{
		BeginTask("RemoveComponents");

		for (EntityID entity : mEntities)
		{
			mRegistry.erase<Position>(entity);
			mRegistry.erase<Velocity>(entity);
		}

		EndTask();
	}

	void Benchmark::UpdateComponent()
	{
	}

	void Benchmark::UpdateComponents()
	{
	}

	void Benchmark::BeginTask(const std::string& taskName)
	{
		LE_ASSERT(!mCurrentTask, "Active task already started!");

		mCurrentTask = new BenchmarkData(taskName);
	}

	void Benchmark::EndTask()
	{
		LE_ASSERT(mCurrentTask, "No active task!");

		auto timestep = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - mCurrentTask->start).count();
		mTimings[mCurrentTask->name] += timestep;

		delete mCurrentTask;
		mCurrentTask = nullptr;
	}
}