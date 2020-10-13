#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>

#include "process.h"

const int p0 = 2;
const int cpu_time_per_kwant = 8;

void print_proc(std::vector<Process*>& all_proc);
void print_state_proc(std::vector<Process*>& all_proc, int count_kwants);
void print_stat(std::vector<Process*>& all_proc);

void SJF(std::vector<Process*>& all_proc)
{
	bool isRun{ false };
	Process* runnable{ nullptr };
	for (auto& i : all_proc)
	{
		if (i->getState() == Process::state_t::run)
		{
			isRun = true;
			runnable = i;
			break;
		}
	}

	if (isRun)
	{
		if (runnable->getInRunTime() >= runnable->getLeadTime()) /*выполнился ли процесс?*/
		{
			runnable->setState(Process::state_t::done);
			isRun = false;
		}
	}
	
	if (!isRun) /*выбираем процесс для исполнения*/
	{
		runnable = nullptr;
		int min_lead = INT_MAX;
		for (auto& i : all_proc)
		{
			if (i->getLeadTime() < min_lead && i->getState() == Process::state_t::ready)
			{
				runnable = i;
				min_lead = i->getLeadTime();
			}
		}

		if (min_lead != INT_MAX)
		{
			runnable->setState(Process::state_t::run);
		}
	}
}

void HLRR(std::vector<Process*> all_proc)
{
	bool isRun{ false };
	Process* runnable{ nullptr };
	for (auto& i : all_proc)
	{
		if (i->getState() == Process::state_t::run)
		{
			isRun = true;
			runnable = i;
			break;
		}
	}

	if (isRun)
	{
		if (runnable->getInRunTime() >= runnable->getLeadTime()) /*выполнился ли процесс?*/
		{
			runnable->setState(Process::state_t::done);
		}
		else
		{
			runnable->setState(Process::state_t::ready);
		}
	}

	for (auto& i : all_proc)
	{
		i->del2CpuTime();
		i->setPriority(p0 + i->getCpuTime() / 2);
	}
	std::sort
	(
		all_proc.begin(), all_proc.end(),
		[](const Process* p1, const Process* p2)
		{
			return p1->getPriority() < p2->getPriority();
		}
	);

	bool isReady{ false };
	Process* ready{ nullptr };
	for (auto& i : all_proc)
	{
		if (i->getState() == Process::state_t::ready)
		{
			isReady = true;
			ready = i;
			break;
		}
	}
	if (isReady)
	{
		ready->setState(Process::state_t::run);
		ready->incCpuTime(cpu_time_per_kwant);
	}
}

int main()
{
	system("chcp 65001>nul");
	std::vector<Process*> all_proc;

	/*массив процессов*/
	all_proc.push_back(new Process(10, p0, Process::state_t::ready, 0));
	all_proc.push_back(new Process(6, p0, Process::state_t::ready, 0));
	all_proc.push_back(new Process(5, p0, Process::state_t::not_launched, 6));
	all_proc.push_back(new Process(4, p0, Process::state_t::not_launched, 3));
	all_proc.push_back(new Process(8, p0, Process::state_t::not_launched, 6));
	all_proc.push_back(new Process(6, p0, Process::state_t::not_launched, 1));

	/*Информация о процессах*/
	print_proc(all_proc);
	std::cout << "Выполнение: " << std::endl;
	
	bool done{ false };
	int count_kwants{ 0 };
	while (!done)
	{
		/*алгоритм*/
		//SJF(all_proc);
		HLRR(all_proc);

		/*проверка, можно ли завершать выполнение цикла*/
		done = true;
		for (auto& i : all_proc)
		{
			if (i->getState() != Process::state_t::done)
			{
				done = false;
				break;
			}
		}

		if (!done)
		{
			for (auto& i : all_proc) /*обновление процессов*/
				i->update(count_kwants);
		}

		/*вывод состояния процессов*/
		print_state_proc(all_proc, count_kwants);
		count_kwants++;

		system("pause>nul");
	}

	/*статистика*/
	print_stat(all_proc);

	for (auto& i : all_proc)
		delete i;

	return bool(std::cout);
}

void print_proc(std::vector<Process*>& all_proc)
{
	std::cout << "Процессы: " << std::endl;
	std::cout << "Номера: ";
	for (auto& i : all_proc)
	{
		std::cout << "\t" << i->getId();
	}
	std::cout << std::endl;
	std::cout << "t выполнения: ";
	for (auto& i : all_proc)
	{
		std::cout << "\t" << i->getLeadTime();
	}
	std::cout << std::endl;
	std::cout << "t появления: ";
	for (auto& i : all_proc)
	{
		std::cout << "\t" << i->getAppearanceTime();
	}
	std::cout << std::endl;
}

void print_state_proc(std::vector<Process*>& all_proc, int count_kwants)
{
	std::cout << "\t";
	for (auto& i : all_proc)
	{
		std::cout << "\t" << i->getState();
	}
	std::cout << "\t" << count_kwants << " - квант" << std::endl;
}

void print_stat(std::vector<Process*>& all_proc)
{
	std::cout << "t в системе:";
	for (auto& i : all_proc)
	{
		std::cout << "\t" << i->getInSystemTime();
	}
	std::cout << std::endl;

	std::cout << "Потеряно t:";
	for (auto& i : all_proc)
	{
		std::cout << "\t" << i->getInSystemTime() - i->getLeadTime();
	}
	std::cout << std::endl;

	std::cout << std::setprecision(3);

	std::cout << "Отн. реактив.:";
	for (auto& i : all_proc)
	{
		std::cout << "\t" << float(i->getLeadTime()) / i->getInSystemTime();
	}
	std::cout << std::endl;

	std::cout << "Штрафное отн.:";
	for (auto& i : all_proc)
	{
		std::cout << "\t" << float(i->getInSystemTime()) / i->getLeadTime();
	}
	std::cout << std::endl;
}
