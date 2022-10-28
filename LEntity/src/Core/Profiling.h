#pragma once

#include "Reflection.h"
#include "Types.h"

namespace LEnt {

	struct ProfileResult
	{
		std::string name;
		long long start, end;
		std::thread::id threadID;
	};

	struct ProfilingSession
	{
		std::string name;
	};

	class Profiler
	{
	public:
		Profiler(const Profiler&) = delete;
		Profiler(Profiler&&) = delete;

		void BeginSession(const std::string& name, const std::string& filepath = "profile.json")
		{
			std::scoped_lock lock(mMutex);
			if (mCurrentSession)
				InternalEndSession();

			mOutputStream.open(filepath);
			if (!mOutputStream.is_open())
				return;

			mCurrentSession = new ProfilingSession({ name });
			WriteHeader();
		}

		void EndSession()
		{
			std::scoped_lock lock(mMutex);
			InternalEndSession();
		}

		void WriteProfile(const ProfileResult& result)
		{
			std::stringstream json;

			json << ",{";
			json << "\"cat\":\"function\",";
			json << "\"dur\":" << (result.end - result.start) << ',';
			json << "\"name\":\"" << result.name << "\",";
			json << "\"ph\":\"X\",";
			json << "\"pid\":0,";
			json << "\"tid\":" << result.threadID << ",";
			json << "\"ts\":" << result.start;
			json << "}";

			std::scoped_lock lock(mMutex);
			if (!mCurrentSession)
				return;

			mOutputStream << json.str();
			mOutputStream.flush();
			
		}

		static Profiler& Get() {
			static Profiler instance;
			return instance;
		}

	private:
		Profiler()
			: mCurrentSession(nullptr)
		{
		}

		~Profiler()
		{
			EndSession();
		}

		void WriteHeader()
		{
			mOutputStream << "{\"otherData\": {},\"traceEvents\":[{}";
			mOutputStream.flush();
		}

		void WriteFooter()
		{
			mOutputStream << "]}";
			mOutputStream.flush();
		}

		// Note: you must already own lock on mMutex before
		// calling InternalEndSession()
		void InternalEndSession()
		{
			if (!mCurrentSession)
				return;

			WriteFooter();
			mOutputStream.close();
			delete mCurrentSession;
			mCurrentSession = nullptr;
		}

	private:
		std::mutex mMutex;
		ProfilingSession* mCurrentSession;
		std::ofstream mOutputStream;
	};

	class ProfilingTimer
	{
	public:
		ProfilingTimer(const char* name)
			: mName(name), mStopped(false)
		{
			mStartTimepoint = Clock::now();
		}

		~ProfilingTimer()
		{
			if (!mStopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = Clock::now();

			long long start = Time::time_point_cast<Time::microseconds>(mStartTimepoint).time_since_epoch().count();
			long long end = Time::time_point_cast<Time::microseconds>(endTimepoint).time_since_epoch().count();

			Profiler::Get().WriteProfile({ mName, start, end, std::this_thread::get_id() });

			mStopped = true;
		}
	private:
		const char* mName;
		Timepoint mStartTimepoint;
		bool mStopped;
	};

	namespace ProfilingUtils {

		template <size_t N>
		struct ChangeResult
		{
			char data[N];
		};

		template <size_t N, size_t K>
		constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}
	}
}

#define LE_PROFILE_ENABLE 0

#if LE_PROFILE_ENABLE
#define LE_PROFILE_BEGIN_SESSION(name, filepath) ::LEnt::Profiler::Get().BeginSession(name, filepath)
#define LE_PROFILE_END_SESSION() ::LEnt::Profiler::Get().EndSession()
#define LE_PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::LEnt::ProfilingUtils::CleanupOutputString(name, "__cdecl ");\
												   ::LEnt::ProfilingTimer timer##line(fixedName##line.data)
#define LE_PROFILE_SCOPE_LINE(name, line) LE_PROFILE_SCOPE_LINE2(name, line)
#define LE_PROFILE_SCOPE(name) LE_PROFILE_SCOPE_LINE(name, __LINE__)
#define LE_PROFILE_FUNCTION() LE_PROFILE_SCOPE(LE_FUNC_SIG)
#else
#define LE_PROFILE_BEGIN_SESSION(name, filepath)
#define LE_PROFILE_END_SESSION()
#define LE_PROFILE_SCOPE(name)
#define LE_PROFILE_FUNCTION()
#endif 