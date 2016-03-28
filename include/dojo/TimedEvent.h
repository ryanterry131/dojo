#pragma once

namespace Dojo {
	class WorkerPool;
	class TimedEventImpl;

	class TimedEvent {
		friend class EventManager;
	public:
		static void runTimedEvents(TimePoint now);
		
		TimedEvent();
		~TimedEvent();

		TimedEvent(TimedEvent&&);
		TimedEvent& operator=(TimedEvent&&);
		
		TimedEvent(const TimedEvent&) = delete;
		TimedEvent& operator=(const TimedEvent&) = delete;

		void start(
			Duration interval,
			AsyncTask task, 
			AsyncCallback callback = {},
			optional_ref<WorkerPool> targetPool = {}
		);

	protected:
		
		Unique<TimedEventImpl> mImpl;
	};
}
