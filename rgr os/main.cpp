/*
У парикмахера есть одно рабочее место и приемная с несколькими стульями. 
Когда парикмахер заканчивает подстригать клиента, 
он отпускает клиента и затем идет в приёмную, чтобы посмотреть, 
есть ли там ожидающие клиенты. Если они есть, он приглашает одного из них и стрижет его. 
Если ждущих клиентов нет, он возвращается к своему креслу и спит в нем.
Каждый приходящий клиент смотрит на то, что делает парикмахер.
Если парикмахер спит, то клиент будит его и садится в кресло. 
Если парикмахер работает, то клиент идет в приёмную. 
Если в приёмной есть свободный стул, клиент садится и ждёт своей очереди. 
Если свободного стула нет, то клиент уходит
*/


#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

int main()
{
	system("chcp 65001>nul");

	std::queue<int> visitors{};    /*очередь посетителей*/
	size_t max_free_chairs{ 5 };   /*максимальное количество кресел*/

	std::mutex m{};
	std::condition_variable c_v{}; /*для пробуждения и засыпания брадобрея*/
	bool all_cut{ false };         /*все пострижены*/
	bool visitor_came{ false };    /*посетитель пришел*/
	const int visitor_count{ 100 };  /*количество посетителей*/

	auto v_function{ [&]() /*деятельность посетителей*/
	{
		for (int i = 0; i < visitor_count; ++i) /*симуляция прихода посетителей*/
		{
			int time{ rand() % 40 };
			std::this_thread::sleep_for(std::chrono::milliseconds(time));  /*симуляция прихода посетителей*/

			std::unique_lock<std::mutex> lock(m); /*блокируем работу брадобрея*/
			if (visitors.size() < max_free_chairs)
			{
				visitors.push(i);    /*добавляем посетителя в очередь*/
				visitor_came = true; /*посетитель пришел*/
				std::cout << "Cвободных мест: " << max_free_chairs - visitors.size() 
					<< ". Пришел посетитель " << i << " через время " << time << " мс. " << std::endl;
				c_v.notify_one();    /*будим поток брадобрея*/
			} /*иначе клиент просто уходит :(*/
			else
			{
				std::cout << "Cвободных мест: " << max_free_chairs - visitors.size() 
					<< ". Пришел посетитель " << i << " через время " << time 
					<< " мс. Свободных мест нет, посетитель уходит" << std::endl;
			}
		}
		all_cut = true;    /*больше посетителей не будет*/
		c_v.notify_one();  /*пробуждаем поток брадобрея, чтобы он заснул навсегда :| */
	} };

	auto s_function{ [&]() /*деятельность брадобрея*/
	{
		static size_t trimmed{};
		std::unique_lock<std::mutex> lock(m); /*блокируем всех посетителей*/
		while (!all_cut) /*до тех пор, пока все не пострижены*/
		{
			c_v.wait(lock); /*спим, пока посетитель не разбудит*/
			while (!visitors.empty()) /*пока очередь не пуста*/
			{
				int visitor{ visitors.front() }; /*запоминаем посетителя*/
				visitors.pop();                  /*убираем его из очереди*/
				std::cout << "Свободных мест: " << max_free_chairs - visitors.size() 
					<< ". Брадобрей начал стричь " << visitor << " посетителя" << std::endl;

				lock.unlock();  /*место в очереди может занять какой-нибудь посетитель*/
				int time = 25;
				std::this_thread::sleep_for(std::chrono::milliseconds(time)); /*время стрижки*/
				trimmed++;
				lock.lock();

				std::cout << "Свободных мест: " << max_free_chairs - visitors.size() 
					<< ". Посетитель " << visitor << " пострижен за время " << time << " мс." 
					<< ((visitors.empty()) ? " Брадобрей спит" : "") << std::endl;
			}
			visitor_came = false; /*посетителей пока нет*/
		}
		std::cout << "Пострижено " << trimmed << " посетителей" << std::endl;
	} };

	std::thread stylist(s_function);
	std::thread visitor(v_function);

	visitor.join();
	stylist.join();

	return 0;
}