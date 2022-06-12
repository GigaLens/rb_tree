#ifndef RB_TREE_H
#define RB_TREE_H

#define RB_OK   0  /* 红黑树操作成功 */
#define RB_ERR  1  /* 红黑树操作失败 */

/* 比较函数原型 */
typedef int (*COMPARE_FUNC)(const void *key1, const void *key2);

/* 红黑树结构体申明 */
typedef struct tagRbTree RB_TREE_S;

/* 创建红黑树 */
RB_TREE_S *RbTreeCreate(COMPARE_FUNC pCmpFunc);

/* 插入数据结点 */
int RbTreeAdd(RB_TREE_S *pRbTree, void *key, unsigned int keyLen, void *data,
    unsigned int dataLen);

/* 查找数据 */
int RbTreeGetData(RB_TREE_S *pRbTree, void *key, void *dataBuff, unsigned int buffLen);

/* 删除数据结点 */
int RbTreeRemove(RB_TREE_S *pRbTree, void *key);

/* 销毁红黑树 */
void RbTreeDestroy(RB_TREE_S *pRbTree);

#endif

