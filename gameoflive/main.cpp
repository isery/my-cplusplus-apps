//Georg Eschbacher
//Michael Hettegger

#include <iostream>
#include "time.h"
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <math.h>
#include <thread>



timespec diff(timespec start, timespec end)
{
	timespec temp;
	if((end.tv_nsec - start.tv_nsec) < 0)
	{
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec-start.tv_nsec;
	}
	else
	{
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}

	return temp;
}

double getSeconds(timespec time1,timespec time2)
{
	return ((diff(time1, time2).tv_sec) + ((diff(time1, time2).tv_nsec)*pow(10, -9)));
}

std::string generateFilename(int n)
{
	std::stringstream out;
	out << "./frames/";
	if(n<10)
		out << "00";
	else if(n<100)
		out << "0";
		
	out << n << ".pbm";

	return out.str();
}

void getNextPBM(std::vector<std::vector<bool>>* board, int width, int height, int n)
{
	std::ofstream output;
	output.open(generateFilename(n), std::fstream::trunc);

	output << "P1\n" << width << ' ' << height << std::endl;
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++)
			output << (*board)[i][j] << ' ' ;
		output << std::endl;
	}
}

void readStartConfig(std::vector<std::vector<bool>> &currentGenerationBoard, std::vector<std::vector<bool>> &nextGenerationBoard, int& configWidth, int& configHeight)
{
	// read file input
	std::fstream startConfigFile;
	startConfigFile.open("./start.pbm", std::fstream::in);
	
	std::string input;
	
	//returns line with P1
	std::getline(startConfigFile, input);
	
	// returns line with Width and Height
	std::getline(startConfigFile, input);
	int widhtAndHeight = input.find(' ', 0);
	configWidth = atoi(input.substr(0, widhtAndHeight).c_str());
	configHeight = atoi(input.substr(widhtAndHeight, input.length()).c_str());

	// create board
	currentGenerationBoard.reserve(configHeight);
	nextGenerationBoard.reserve(configHeight);
	for(int i=0; i<configHeight; i++)
	{
		currentGenerationBoard[i].reserve(configWidth);
		nextGenerationBoard[i].reserve(configWidth);
	}

	for(int i=0; i<configHeight; i++)
	{
		std::string line;
		std::getline(startConfigFile, line);

		for(int j=0; j<configWidth; j++)
		{
			nextGenerationBoard[i][j] = false;

			char curr = line.at(j*2);
			
			if(curr == '0')
				currentGenerationBoard[i][j] = false;
			else
				currentGenerationBoard[i][j] = true;
		}
	}
}

int getLivingNeighborCount(int posY, int posX, std::vector<std::vector<bool>>* board, int configWidth, int configHeight)
{
	int count = 0;
	for(int i = posY-1; i<=posY+1; i++)
	{
		for(int j = posX-1; j<=posX+1; j++)
		{	
			if( (j>=0) && (i>=0) && (j<configWidth) && (i<configHeight))
			{
				if(((*board)[i][j] == true) && ((i!=posY) || (j!=posX)) )
					++count;
			}
		}
	}
	return count;
}

bool gameOfLifeRule(int livingNeighborCount, bool alive)
{
	if(livingNeighborCount == 3)
		return true;
	else if(alive && (livingNeighborCount == 2))
		return true;
	
	return false;
}

void work(std::vector<std::vector<bool>>* inputBoard, std::vector<std::vector<bool>>* outputBoard, int configWidth, int configHeight, int startLine, int MAX_THREADS, bool *end)
{
	for(int i=startLine; i < configHeight; i += MAX_THREADS)
	{
		for(int j=0; j < configWidth; j++)
		{
			int neighborCount  = getLivingNeighborCount(i, j, inputBoard, configWidth, configHeight);
			bool alive = gameOfLifeRule(neighborCount, (*inputBoard)[i][j]);

			(*outputBoard)[i][j] = alive;
			
			if(*end && (alive != (*inputBoard)[i][j]))
				*end = false;
		}
	}
}

void calcNextPBM(std::vector<std::vector<bool>>* inputBoard, std::vector<std::vector<bool>>* outputBoard, int configWidth, int configHeight, int MAX_THREADS, bool* end)
{
	*end = true;
	std::vector<std::thread> t;
	for(int i=0; i<MAX_THREADS; i++)
		t.push_back(std::thread(work, inputBoard, outputBoard, configWidth, configHeight, i, MAX_THREADS, end));
	
	for (int i = 0; i < MAX_THREADS; ++i)
    	t[i].join();
}

int main()
{
	int MAX_THREADS = sysconf( _SC_NPROCESSORS_ONLN );
	std::cout << "#CPUs: " << MAX_THREADS << std::endl;
	int ITERATIONS = 100;
	int currentPBM = 0;

	std::vector<std::vector<bool>> currentGenerationBoard;
	std::vector<std::vector<bool>> nextGenerationBoard;
	
	int	configWidth;
	int	configHeight;
	
	bool end = false;

	readStartConfig(currentGenerationBoard, nextGenerationBoard, configWidth, configHeight);
	
	timespec time1, time2;
  	double secs = 0;
    
	while((currentPBM < ITERATIONS) && (!end))
	{
	
		std::vector<std::vector<bool>>* inputBoard;
		std::vector<std::vector<bool>>* outputBoard;

		if(currentPBM%2 == 0)
		{
			outputBoard = &nextGenerationBoard;
			inputBoard = &currentGenerationBoard;
		}
		else
		{
			outputBoard = &currentGenerationBoard;
			inputBoard = &nextGenerationBoard;
		}
		clock_gettime(CLOCK_MONOTONIC, &time1);
		calcNextPBM(inputBoard, outputBoard, configWidth, configHeight, MAX_THREADS, &end);
		clock_gettime(CLOCK_MONOTONIC, &time2);
		secs += getSeconds(time1, time2);
		getNextPBM(outputBoard, configWidth, configHeight, currentPBM);

		++currentPBM;
	}
		
	std::cout<< "secs: "<< secs <<std::endl;
	return 0;
}
