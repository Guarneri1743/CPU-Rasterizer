#include "Application.hpp"

int main()
{
	Guarneri::Application::load_scene("/scenes/lighting_sample.scene");
	Guarneri::Application::play();
	return 0;
}
//
//#include "SafeQueue.hpp"
//#include "ThreadPool.hpp"
//
//struct Test
//{
//	SafeQueue<int> queue;
//};
//
//int main()
//{
//	Test* t = new Test[10];
//	ThreadPool p(8);
//	std::vector<std::future<void>> r1;
//
//	for (int i =0; i < 10000; i++)
//	{
//		r1.emplace_back(p.enqueue([&p, &t, i]
//		{
//			for (int j = 0; j < 1000; j++)
//			{
//				int index = j % 10;
//				t[index].queue.produce(j);
//			}
//		}));
//	}
//
//	for (auto& r : r1)
//	{
//		r.get();
//	}
//
//	delete[] t;
//	return 0;
//}