#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

typedef struct s_block *t_block;

//定义内存块管理结构体
struct s_block {
    size_t size;  /* 数据区大小 */
    t_block prev; /* 指向上个块的指针 */
    t_block next; /* 指向下个块的指针 */
    int free;     /* 是否是空闲块 */
    int padding;  /* 填充4字节，保证meta块长度为8的倍数 */
    void *ptr;    /* Magic pointer，指向data */
    char data[1];  /* 这是一个虚拟字段，表示数据块的第一个字节，长度不应计入meta */
};

#define BLOCK_SIZE 40 /* 由于存在虚拟的data字段，sizeof不能正确计算meta长度，这里手工设置 */

void *first_block = NULL;

/* First fit */
t_block find_block(t_block *last, size_t size) {
    t_block b = first_block;
    while(b && !(b->free && b->size >= size)) {
        *last = b;
        b = b->next;
    }
    return b;
}

//创建新的内存块
t_block extend_heap(t_block last, size_t s) {
    t_block b;
    b = sbrk(0);
    if(sbrk(BLOCK_SIZE + s) == (void *)-1)
        return NULL;
    b->size = s;
    b->next = NULL;
    if(last){
        last->next = b;
		b->prev = last;
	}
    b->free = 0;
	b->ptr = b->data;
    return b;
}

//分裂大的内存块
void split_block(t_block b, size_t s) {
    t_block new;
    new = b->data + s;
    new->size = b->size - s - BLOCK_SIZE ;
    new->next = b->next;
	new->prev = b;
    new->free = 1;
    b->size = s;
    b->next = new;
}

size_t align8(size_t s) {
    if(s & 0x7 == 0)
        return s;
    return ((s >> 3) + 1) << 3;
}

void *malloc(size_t size) {
    t_block b, last;
    size_t s;
    /* 对齐地址 */
    s = align8(size);
    if(first_block) {
        /* 查找合适的block */
        last = first_block;
        b = find_block(&last, s);
        if(b) {
            /* 如果可以，则分裂 */
            if ((b->size - s) >= ( BLOCK_SIZE + 8))
                split_block(b, s);
            b->free = 0;
        } else {
            /* 没有合适的block，开辟一个新的 */
            b = extend_heap(last, s);
            if(!b)
                return NULL;
        }
    } else {
        b = extend_heap(NULL, s);
        if(!b)
            return NULL;
        first_block = b;
    }
    return b->data;
}

void *calloc(size_t number, size_t size) {
    size_t *new;
    size_t s8, i;
    new = malloc(number * size);
    if(new) {
        s8 = align8(number * size) >> 3;
        for(i = 0; i < s8; i++)
            new[i] = 0;
    }
    return new;
}

t_block get_block(void *p) {
    char *tmp;  
    tmp = p;
	printf("get_block p:%p, tmp:%p \n", p, tmp - BLOCK_SIZE);
		return (p = tmp -= BLOCK_SIZE);
}

int valid_addr(void *p) {
    if(first_block) {
        if(p > first_block && p < sbrk(0)) {
			printf("valid_addr p:%p, ptr:%p \n", p, (get_block(p))->ptr );
			return p == (get_block(p))->ptr;
        }
    }
    return 0;
}

//合并内存块
t_block fusion(t_block b) {
    if (b->next && b->next->free) {
        b->size += BLOCK_SIZE + b->next->size;
        b->next = b->next->next;
        if(b->next)
            b->next->prev = b;
    }
    return b;
}

void free(void *p) {
	printf("free: %p \n", p);
    t_block b;
    if(valid_addr(p)) {
        b = get_block(p);
        b->free = 1;
        if(b->prev && b->prev->free)
            b = fusion(b->prev);
        if(b->next)
            fusion(b);
        else {
            if(b->prev)
                b->prev->next = NULL;
            else first_block = NULL;
            brk(b);
        }
    }
}

void copy_block(t_block src, t_block dst) {
	size_t *sdata, *ddata;
	size_t i;
	sdata = src->ptr;
	ddata = dst->ptr;
	for(i = 0; (i * 8) < src->size && (i * 8) < dst->size; i++)
		ddata[i] = sdata[i];
}

void *realloc(void *p, size_t size) {
    size_t s;
    t_block b, new;
    void *newp;
    if (!p)
        /* 根据标准库文档，当p传入NULL时，相当于调用malloc */
        return malloc(size);
    if(valid_addr(p)) {
        s = align8(size);
        b = get_block(p);
        if(b->size >= s) {
            if(b->size - s >= (BLOCK_SIZE + 8))
                split_block(b,s);
        } else {
            /* 看是否可进行合并 */
            if(b->next && b->next->free
                    && (b->size + BLOCK_SIZE + b->next->size) >= s) {
                fusion(b);
                if(b->size - s >= (BLOCK_SIZE + 8))
                    split_block(b, s);
            } else {
                /* 新malloc */
                newp = malloc (s);
                if (!newp)
                    return NULL;
                new = get_block(newp);
                copy_block(b, new);
                free(p);
                return(newp);
            }
        }
        return (p);
    }
    return NULL;
}

void print_block(){

	t_block b = first_block;
	
	printf("print block start. \n");

	while(b != NULL){
		printf("size:%d, prev:%p, next:%p, free:%d, ptr:%p, data:%p \n",
				b->size, b->prev, b->next, b->free, &b->size, b->data);
		b = b->next;
	}

	printf("print block end. \n");
}


int main(int argc, char *argv[])
{
	int i, size;
	void * ptr[10] = {NULL};
	
	//random malloc
	for(i = 0; i < 10; i++ ){
		size = random() % 100;		
		ptr[i] = malloc(size);
		printf("ptr = %p \n", ptr[i]);
	}

	print_block();

	//random free
	free(ptr[2]);
	free(ptr[7]);

	print_block();
	
	return 0;
}
