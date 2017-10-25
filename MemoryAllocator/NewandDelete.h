#pragma once
class MyHeapManager;
class Allocator;

const int g_DefaultAlignment = 4;
void * operator new(size_t i_size, MyHeapManager * i_pHeap);
void operator delete(void * i_ptr, MyHeapManager * i_pHeap);

void * operator new(size_t i_size, MyHeapManager * i_pHeap, unsigned int i_align);
void operator delete(void * i_ptr, MyHeapManager * i_pHeap, unsigned int i_align);

void * operator new(size_t i_size, Allocator * i_pHeap);
void operator delete(void * i_ptr, Allocator * i_pHeap);

void * operator new(size_t i_size, Allocator * i_pHeap, unsigned int i_align);
void operator delete(void * i_ptr, Allocator * i_pHeap, unsigned int i_align);
