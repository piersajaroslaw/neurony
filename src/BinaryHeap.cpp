/*
 * BinaryHeap.cpp
 *
 *  Created on: Mar 29, 2011
 *      Author: piersaj
 *
 *  Binary heap
 *  a textbook sstructure for Dijkstra algorithm...
 *  but ffs i'm so fed up with writing this...
 *
 *  2011-03-29
 *  	just it write and forget
 *  	again in english...
 *  2011-03-30
 *  	building heap...
 *  2011-03-31
 *  	... minor fixes... so far
 *  	... hardcore debugging... so far :P
 *  	... holy christmass, how ever have i ever passed my programming classes?!
 *  	... BRAAAIIINS!
 *
 *
 */

#include "BinaryHeap.h"
#include <iostream>
#include "../src/logger.h"

BinaryHeap::BinaryHeap(int maxlen):
	len(maxlen)
{
	currentLen = 0;

	key = new int[len];
	weigth = new int[len];
	whereIs = new int[len];
}	// this


BinaryHeap::~BinaryHeap(){
	delete[] key;
	delete[] weigth;
	delete[] whereIs;
}	// ~this

int BinaryHeap::parent(int index){
	if (index >=1){
		return (index-1) / 2;
	} 	else {
		// fatal error!
		return -1;
	}	// if
}	// parent

int BinaryHeap::leftSonIndex(int index){
	int val = 2*index+1;
	if (val >=currentLen){
		return -1;
	} else {
		return val;
	}	// fi
}	// lson

int BinaryHeap::rightSonIndex(int index){
	int val = 2*index+2;
	if (val >=currentLen){
		return -1;
	} else {
		return val;
	}	// fi
}	// rson

void BinaryHeap::heapyfy(int atIndex){
	if (atIndex >= currentLen){
		std::cerr << "BH.heapify(): Error: index out of range!\n";
		return;
	}	// fi

	// real heapify is top-down
	int smallestWeigth  = weigth[atIndex];
	int smallestInd  = atIndex;

	if (leftSonIndex(atIndex) != -1 && weigth[leftSonIndex(atIndex)] < smallestWeigth){
		smallestWeigth = weigth[leftSonIndex(atIndex)];
		smallestInd = leftSonIndex(atIndex);
	}	// fi

	if (rightSonIndex(atIndex) != -1 && weigth[rightSonIndex(atIndex)] < smallestWeigth){
		smallestWeigth = weigth[rightSonIndex(atIndex)];
		smallestInd = rightSonIndex(atIndex);
	}	// fi

	if (smallestInd != atIndex){
		int temp = weigth[smallestInd];
		weigth[smallestInd] = weigth[atIndex];
		weigth[atIndex] = temp;

		// swap position references
		whereIs[key[smallestInd]] = atIndex;
		whereIs[key[atIndex]] = smallestInd;

		temp = key[smallestInd];
		key[smallestInd] = key[atIndex];
		key[atIndex] = temp;
		heapyfy(smallestInd);
	}	// fi
}	// heapify

bool BinaryHeap::isEmpty(){
	return currentLen == 0;
}

void BinaryHeap::push(int k, int w){
	if (currentLen == len){
		// fatal error
		std::cerr << "BinaryHeap.push(): fatal error - out of memory!\n\n";
		return;
	}	// fi
	currentLen ++;
	key[currentLen-1] = k;
	weigth[currentLen-1] = w;
	whereIs[k] = currentLen-1;

	int pos = currentLen-1;
	int par = parent(currentLen-1);
	int temp = -1;
	while (par >=0 && key[pos] < key[par]){
		temp = weigth[pos];
		weigth[pos] = weigth[par];
		weigth[par] = temp;
		// swap position references
		whereIs[key[par]] = pos;
		whereIs[key[pos]] = par;
		// swap keys
		temp = key[pos];
		key[pos] = key[par];
		key[par] = temp;

		// go up one level
		pos = par;
		par = parent(pos);
	}	// while

	currentLen++;
}	// push

int BinaryHeap::popMinKey(){
	if (isEmpty()){
		std::cerr<< "BinaryHeap.pop(): fatal error: the heap is empty\n";
		return -1;
	}	// if
	int ret = key[0];

	whereIs[key[0]]  =-1;

	if (currentLen >= 2){
		key[0] = key[currentLen-1];
		weigth[0] = weigth[currentLen-1];
		whereIs[key[currentLen-1]] = 0;
		currentLen--;
		heapyfy(0);
	} else {
		currentLen = 0;
	}	// else
	return ret;
}	// int

void BinaryHeap::changeWeigth(int k, int newWeight){
	int index = whereIs[k];
	if (index == -1){
		logJP << "BinaryHeap.changeWeigth(): key "<< k <<" not found\n";
		push(k, newWeight);
		return;
	}	// fi

	if (key[index] != k){
		logJP  << "BinaryHeap.changeWeigth(): whereIs[] is no longer consisstant... "
				<< "Huston we have a problemm! k= " << k << ",  key[whereis[k]] = "
				<< key[index]<<"\n";
	}	// fi

	if (weigth[index] == newWeight){
		// skip
		return;
	}	else if (weigth[index] < newWeight){
		// growing weigth NYI
		logJP << "BinaryHeap.changeWeigth(): new Weigth is larger than old "
				<< newWeight <<" > " << weigth[index] <<"\n";
		heapyfy(index);
		return;
	} else /* (weigth[index] > newWeight) */ {
		// by reducing weigth we can bring it higher

		weigth[index] = newWeight;

		int pos = index;
		int par = parent(index);
		int temp = -1;
		while (par >= 0 && weigth[pos] < weigth[par]){
			// swap position references
			whereIs[key[par]] = pos;
			whereIs[key[pos]] = par;

			temp = weigth[pos];
			weigth[pos] = weigth[par];
			weigth[par] = temp;

			// swap keys
			temp = key[pos];
			key[pos] = key[par];
			key[par] = temp;

			// up one level
			pos = par;
			par = parent(par);
		}	// while
	}	// fi
}	// change weigth


void BinaryHeap::buildHeap(int len_, int * keys, int * weigths){
	if (currentLen != 0){
		std::cerr << "BinaryHeap.buildHeap(): the heap is already built!\n\n";
		return;
	}	// fi

	for (int i=0; i<len_; i++){
		key[i] = keys[i];
		weigth[i] = weigths[i];
		whereIs[key[i]] = i;
	}	// fi
	currentLen = len_;

	for (int i=currentLen/2+1; i>=0; i--){
		heapyfy(i);
	}	// for
}	// bulild heap

void BinaryHeap::clear(){
	currentLen = 0;
}	// clear

void BinaryHeap::print(){
	for (int i=0; i<currentLen; i++){
		logJP << "k = " << key[i] << "  w = " << weigth[i] <<"\n";
	}	// for
	logJP<<"\n\n\n";
}	// print
