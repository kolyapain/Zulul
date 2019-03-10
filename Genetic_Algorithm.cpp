#include <iostream>
#include <time.h>
#include <algorithm>

using namespace std;

const int DNA_Msize = 100;
const int DNA_Mcnt = 500;
const int DNA_MCcnt = 1500;
struct DNA
{
	double chromosomes[DNA_Msize];
	double fitness;
};

bool operator<(const DNA & d1, const DNA & d2)
{
	if (d1.fitness < d2.fitness)
		return 1;
	return 0;
}

DNA Main_population[DNA_Mcnt];
DNA Tournament_population[DNA_Mcnt];
DNA Crossover_population[DNA_MCcnt];
DNA Res_population[DNA_Mcnt];
int Player_ids[DNA_Mcnt];
double Desired_result = 0.0;

double Fx(double * a, int size)
{
	double res = 0.0;

	for (int i = 0; i < size; i++)
		res += a[i] * a[i];

	/*res += 10.0 * size;
	for (int i = 0; i < size; i++)
		res += a[i] * a[i] - 10 * cos(2 * 3.14 * a[i]);*/

	return res;
	//return a[0] + 2.0 * a[1] + 3.0 * a[2] + 4.0 * a[3];
}

void calc_fitness(DNA * dna,  int cnt, int size)
{
	for (int i = 0; i < cnt; i++)
		dna[i].fitness = fabs(Desired_result - Fx(dna[i].chromosomes, size));
}

int get_winner(DNA * dna, int * ids, int size)
{
	int winner = ids[0];
	for (int i = 1; i < size; i++)
		if (dna[winner].fitness > dna[ids[i]].fitness)
			winner = ids[i];
	return winner;
}

void Generate_population(int cnt, int size, double minv, double maxv)
{
	for (int i = 0; i < cnt; i++)
		for (int j = 0; j < size; j++)
			Main_population[i].chromosomes[j] = minv + rand() % (int)(maxv - minv);
	calc_fitness(Main_population, cnt, size);
}

void Tournament(int cnt, int size, int players_cnt)
{
	for (int i = 0; i < cnt; i++)
	{
		for (int j = 0; j < players_cnt; j++)
			Player_ids[j] = rand() % cnt;
		int id = get_winner(Main_population, Player_ids, players_cnt);
		for (int j = 0; j < size; j++)
			Tournament_population[i].chromosomes[j] = Main_population[id].chromosomes[j];
		Tournament_population[i].fitness = Main_population[id].fitness;
	}
}

int Crossover(int cnt, int size)
{
	sort(Tournament_population, Tournament_population + cnt);
	int cid = 0;
	for (int i = 0; i < cnt && cid < DNA_MCcnt - 3; i++)
	{
		for (int j = 0; j < cnt && cid < DNA_MCcnt - 3; j++)
		{
			if (i == j)
				continue;
			for (int k = 0; k < size; k++)
			{
				Crossover_population[cid].chromosomes[k] = (Tournament_population[i].chromosomes[k] + Tournament_population[j].chromosomes[k]) / 2;
				Crossover_population[cid+1].chromosomes[k] = (3 * Tournament_population[i].chromosomes[k] - Tournament_population[j].chromosomes[k]) / 2;
				Crossover_population[cid+2].chromosomes[k] = (3 * Tournament_population[j].chromosomes[k] - Tournament_population[i].chromosomes[k]) / 2;
			}
			cid += 3;
		}
	}
	return cid;
}

void Mutation(int cnt, int size, double minv, double maxv)
{
	for (int i = 0; i < cnt; i++)
	{
		Crossover_population[i].chromosomes[rand() % size] = minv + rand() % (int)(maxv - minv);
		/*for (int j = 0; j < size; j++)
		{
			Crossover_population[i].chromosomes[j] = minv + rand() % (int)(maxv - minv);
		}*/
	}
	calc_fitness(Crossover_population, cnt, size);
}

int Elitism(int mcnt, int ccnt, int size)
{
	sort(Crossover_population, Crossover_population + ccnt);
	sort(Main_population, Main_population + mcnt);
	int i = 0;
	int ic = 0; 
	int im = 0;
	for (; im < mcnt && ic < ccnt && i < DNA_Mcnt; i++)
	{
		if (Crossover_population[ic].fitness < Main_population[im].fitness)
		{
			for (int j = 0; j < size; j++)
				Res_population[i].chromosomes[j] = Crossover_population[ic].chromosomes[j];
			Res_population[i].fitness = Crossover_population[ic].fitness;
			ic++;
		}
		else
		{
			for (int j = 0; j < size; j++)
				Res_population[i].chromosomes[j] = Main_population[im].chromosomes[j];
			Res_population[i].fitness = Main_population[im].fitness;
			im++;
		}
	}

	if (i != DNA_Mcnt)
	{
		for (; i < DNA_Mcnt && ic < ccnt; i++)
		{
			for (int j = 0; j < size; j++)
				Res_population[i].chromosomes[j] = Crossover_population[ic].chromosomes[j];
			Res_population[i].fitness = Crossover_population[ic].fitness;
			ic++;
		}

		for (; i < DNA_Mcnt && im < mcnt; i++)
		{
			for (int j = 0; j < size; j++)
				Res_population[i].chromosomes[j] = Main_population[im].chromosomes[j];
			Res_population[i].fitness = Main_population[im].fitness;
			im++;
		}
	}
	for (int k = 0; k < i; k++)
	{
		for (int j = 0; j < size; j++)
			Main_population[k].chromosomes[j] = Res_population[k].chromosomes[j];
		Main_population[k].fitness = Res_population[k].fitness;
	}
	return i;
}

int check(int size)
{
	for (int i = 0; i < size; i++)
		if (Main_population[i].fitness == 0.0)
			return i;
	return -1;
}

int main()
{
	srand(time(0));
	int chr_cnt = 5;
	int chr_size = 50;
	double minv = -100;
	double maxv = 100;
	int players = 2;
	Desired_result = 0.0;
	Generate_population(chr_cnt, chr_size, minv, maxv);
	for (int iters = 0; iters < 10000; iters++)
	{
		Tournament(chr_cnt, chr_size, players);
		int ccnt = Crossover(chr_cnt, chr_size);
		Mutation(ccnt, chr_size, minv, maxv);
		chr_cnt = Elitism(chr_cnt, ccnt, chr_size);
		int res = check(chr_cnt);
		if (res != -1)
		{
			double va = Fx(Main_population[res].chromosomes, chr_size);
			cout << "iteration : " << iters << endl;
			for (int i = 0; i < chr_size; i++)
				cout << Main_population[res].chromosomes[i] << "  ";
			cout << endl;
			return 0;
		}
		else
		{
			cout << iters << endl;
			for (int i = 0; i < 10; i++)
				cout << Main_population[i].fitness << endl;
		}
	}
	
	return 0;
}
