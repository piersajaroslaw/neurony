/*
 * BinaryHeap.h
 *
 *  Created on: Mar 29, 2011
 *      Author: piersaj
 *
 *  2011-03-29
 *  	a heap for Dijkstra algorithm
 *  	has a min as a priority
 *  	should be able to build, insert, pop lowest nodes
 *  	change weigth of the node (by key not just index)! (this is tricky)
 */

#ifndef BINARYHEAP_JP_H_
#define BINARYHEAP_JP_H_

class BinaryHeap{
	const int len;
	int *key;
	int *weigth;
	int currentLen;
	int * whereIs;

public:
	BinaryHeap(int maxlen);
	~BinaryHeap();

	void push(int k, int weight);
	int popMinKey();
	bool isEmpty();
	void clear();

	void changeWeigth(int key, int newWeight);
	void buildHeap(int len_, int * keys, int * weights);
	void print();

private:
	void heapyfy(int atIndex);
	int parent(int index);
	int leftSonIndex(int index);
	int rightSonIndex(int index);
};

#endif
