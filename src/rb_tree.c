#include "rb_tree.h"
#include <string.h>
#include <stdlib.h>

#define RB_BLACK 0
#define RB_RED   1

/* 红黑树结点定义 */
typedef struct tagRbNode {
    struct tagRbNode *parent;  /* 父结点指针 */
    struct tagRbNode *left;    /* 左子女指针 */
    struct tagRbNode *right;   /* 右子女指针 */
    int bh;                    /* 结点黑高度 */
    int color;                 /* 结点颜色, 黑或红 */
    void *key;                 /* 键值 */
    void *data;                /* 数据 */
} RB_NODE_S;

/* 红黑树结构体定义 */
typedef struct tagRbTree {
    RB_NODE_S *root;            /* 根结点指针 */
    COMPARE_FUNC cmpFunc;       /* 键值比较函数 */
} RB_TREE_S;

/* 创建红黑树 */
RB_TREE_S *RbTreeCreate(COMPARE_FUNC pCmpFunc)
{
    RB_TREE_S *pTree;

    if (pCmpFunc == NULL) {
        return NULL;
    }

    pTree = (RB_TREE_S *)malloc(sizeof(RB_TREE_S));
    if (pTree == NULL) {
        return NULL;
    }

    pTree->root = NULL;
    pTree->cmpFunc = pCmpFunc;
    return pTree;
}

/* 左旋 */
void RbLeftRotate(RB_NODE_S *pNode, RB_TREE_S *pTree)
{
    /* 先记录pNode的父结点以及pNode是左子女还是右子女 */
    RB_NODE_S **ppChild;
    RB_NODE_S *pRight = pNode->right;
    RB_NODE_S *pParent = pNode->parent;

    if (pParent == NULL) {
        /* 当前结点为根结点 */
        ppChild = &(pTree->root);
    } else {
        if (pParent->left == pNode) {
            ppChild = &(pParent->left);
        } else {
            ppChild = &(pParent->right);
        }
    }

    pNode->right = pRight->left;
    if(pRight->left != NULL) {
        pRight->left->parent = pNode;
    }

    pRight->left = pNode;
    pNode->parent = pRight;

    *ppChild = pRight;
    pRight->parent = pParent;

    return;
}

/* 右旋 */
void RbRightRotate(RB_NODE_S *pNode, RB_TREE_S *pTree)
{
    /* 先记录pNode的父结点以及pNode是左子女还是右子女 */
    RB_NODE_S **ppChild;
    RB_NODE_S *pLeft = pNode->left;
    RB_NODE_S *pParent = pNode->parent;

    if (pParent == NULL) {
        /* 当前结点为根结点 */
        ppChild = &(pTree->root);
    } else {
        if (pParent->left == pNode) {
            ppChild = &(pParent->left);
        } else {
            ppChild = &(pParent->right);
        }
    }

    pNode->left = pLeft->right;
    if (pLeft->right != NULL) {
        pLeft->right->parent = pNode;
    }

    pLeft->right = pNode;
    pNode->parent = pLeft;

    *ppChild = pLeft;
    pLeft->parent = pParent;
    return;
}

void RbInsertFixUp(RB_NODE_S *pNode, RB_TREE_S *pTree)
{
    RB_NODE_S *pGrandpa;
    RB_NODE_S *pParent;
    RB_NODE_S *pUncle;
    RB_NODE_S *pCur = pNode;

    while ((pCur->parent != NULL) && (pCur->parent->color == RB_RED)) {
        pParent = pCur->parent;
        pGrandpa = pParent->parent;
        if (pParent == pGrandpa->left) {
            pUncle = pGrandpa->right;
            /* 情况1, 父结点和叔结点都是红色 */
            if ((pUncle != NULL) && (pUncle->color == RB_RED)) {
                pParent->color = RB_BLACK;
                pUncle->color = RB_BLACK;
                pGrandpa->color = RB_RED;
                pCur = pGrandpa;
            } else {
                /* 情况2, 叔结点是黑色, 当前结点是右子女 */
                if (pCur == pParent->right) {
                    pCur = pParent;
                    RbLeftRotate(pCur, pTree);
                }
                /* 情况3, 叔结点是黑色, 当前结点是左子女 */
                pParent->color = RB_BLACK;
                pGrandpa->color = RB_RED;
                RbRightRotate(pGrandpa, pTree);
            }
        } else {
            pUncle = pGrandpa->left;
            /* 情况1, 父结点和叔结点都是红色 */
            if ((pUncle != NULL) && (pUncle->color == RB_RED)) {
                pParent->color = RB_BLACK;
                pUncle->color = RB_BLACK;
                pGrandpa->color = RB_RED;
                pCur = pGrandpa;
            } else {
                /* 情况2, 叔结点是黑色, 当前结点是左子女 */
                if (pCur == pParent->left) {
                    pCur = pParent;
                    RbRightRotate(pCur, pTree);
                }
                /* 情况3, 叔结点是黑色, 当前结点是右子女 */
                pParent->color = RB_BLACK;
                pGrandpa->color = RB_RED;
                RbLeftRotate(pGrandpa, pTree);
            }
        }
    }
    pTree->root->color = RB_BLACK;
    return;
}

RB_NODE_S *RbNodeAlloc(void *key, unsigned int keyLen, void *data, unsigned int dataLen)
{
    void *pKey;
    void *pData;
    RB_NODE_S *pNode;

    pKey = malloc(keyLen);
    if (pKey == NULL) {
        return NULL;
    }

    pData = malloc(dataLen);
    if (pData == NULL) {
        free(pKey);
        return NULL;
    }

    pNode = (RB_NODE_S *)malloc(sizeof(RB_NODE_S));
    if (pNode == NULL) {
        free(pKey);
        free(pData);
        return NULL;
    }

    pNode->parent = NULL;
    pNode->left = NULL;
    pNode->right = NULL;
    pNode->bh = 1;
    pNode->color = RB_RED;
    (void)memcpy(pKey, key, keyLen);
    (void)memcpy(pData, data, dataLen);
    pNode->key = pKey;
    pNode->data = pData;

    return pNode;
}

void RbNodeFree(RB_NODE_S *pNode)
{
    free(pNode->key);
    free(pNode->data);
    free(pNode);
    return;
}

/* 插入数据结点 */
int RbTreeAdd(RB_TREE_S *pTree, void *key, unsigned int keyLen, void *data,
    unsigned int dataLen)
{
    RB_NODE_S *pNode;
    RB_NODE_S *pCur;
    RB_NODE_S *pParent;
    COMPARE_FUNC pCmpFunc;
    void *pKey;
    void *pData;
    int cmpRet;

    /* 参数校验 */
    if ((pTree == NULL) || (pTree->cmpFunc == NULL) || (key == NULL) ||
        (keyLen == 0) || (data == NULL) || (dataLen == 0)) {
        return RB_ERR;
    }

    /* 申请结点 */
    pNode = RbNodeAlloc(key, keyLen, data, dataLen);
    if (pNode == NULL) {
        return RB_ERR;
    }

    if (pTree->root == NULL) {
        /* 根结点为黑色 */
        pNode->color = RB_BLACK;
        pTree->root = pNode;
        return RB_OK;
    }

    pCmpFunc = pTree->cmpFunc;
    pCur = pTree->root;
    while (pCur != NULL) {
        cmpRet = pCmpFunc(key, pCur->key);
        if(cmpRet == 0) {
            /* 结点存在, 直接返回成功 */
            RbNodeFree(pNode);
            return RB_OK;
        }

        pParent = pCur;
        if (cmpRet > 0) {
            pCur = pCur->right;
        } else {
            pCur = pCur->left;
        }
    }

    if (cmpRet < 0) {
        pParent->left = pNode;
    } else {
        pParent->right = pNode;
    }
    pNode->parent = pParent;
    RbInsertFixUp(pNode, pTree);
    
    return RB_OK;
}

/* 查找结点 */
RB_NODE_S *RbNodeFind(RB_TREE_S *pTree, void *key)
{
    RB_NODE_S *pCur = pTree->root;
    COMPARE_FUNC pCmpFunc = pTree->cmpFunc;
    int cmpRet;

    while (pCur != NULL) {
        cmpRet = pCmpFunc(key, pCur->key);
        if (cmpRet == 0) {
            return pCur;
        }
        if (cmpRet < 0) {
            pCur = pCur->left;
        } else {
            pCur = pCur->right;
        }
    }

    return pCur;
}

/* 查找数据 */
int RbTreeGetData(RB_TREE_S *pTree, void *key, void *dataBuff, unsigned int buffLen)
{
    RB_NODE_S *pNode;

    /* 入参校验 */
    if ((pTree == NULL) || (pTree->cmpFunc == NULL) || (key == NULL) ||
        (dataBuff == NULL) || (buffLen == 0)) {
        return RB_ERR;
    }

    pNode = RbNodeFind(pTree, key);
    if (pNode == NULL) {
        return RB_ERR;
    }

    (void)memcpy(dataBuff, pNode->data, buffLen);
    return RB_OK;
}

/* 替换结点 */
void RbNodeTransplant(RB_TREE_S *pTree, RB_NODE_S *pNodeRm, RB_NODE_S *pNodePlant)
{
    /* 替换根结点 */
    if (pNodeRm == pTree->root) {
        pTree->root = pNodePlant;
    } else if (pNodeRm == pNodeRm->parent->left) { /* 左子女 */
        pNodeRm->parent->left = pNodePlant;
    } else { /* 右子女 */
        pNodeRm->parent->right = pNodePlant;
    }

    /* pNodePlant可能为空 */
    if (pNodePlant != NULL) {
        pNodePlant->parent = pNodeRm->parent;
    }

    return;
}

/* 获取子树最小结点 */
RB_NODE_S *RbNodeGetMinimum(RB_NODE_S *pNode)
{
    RB_NODE_S *pNodeMin = pNode;
    while (pNodeMin->left != NULL) {
        pNodeMin = pNodeMin->left;
    }

    return pNodeMin;
}

/* 恢复红黑树性质 */
void RbNodeDeleteFixUp(RB_TREE_S *pTree, RB_NODE_S *pOriNodeFix, RB_NODE_S *pOriFixParent)
{
    RB_NODE_S *pFixBro;
    RB_NODE_S *pNodeFix = pOriNodeFix;
    RB_NODE_S *pFixParent = pOriFixParent;

    while ((pNodeFix != pTree->root) && ((pNodeFix == NULL) || (pNodeFix->color == RB_BLACK))) {
        if (pNodeFix == pFixParent->left) {
            pFixBro = pFixParent->right;
            /* 此处pFixBro一定不为空 */
            if (pFixBro->color == RB_RED) {
                pFixBro->color = RB_BLACK;
                pFixParent->color = RB_RED;
                RbLeftRotate(pFixParent, pTree);
                pFixBro = pFixParent->right;
            }
            if (((pFixBro->left == NULL) || (pFixBro->left->color == RB_BLACK)) &&
                ((pFixBro->right == NULL) || (pFixBro->right->color == RB_BLACK))) {
                pFixBro->color = RB_RED;
                pNodeFix = pFixParent;
                pFixParent = pFixParent->parent;
            } else {
                if ((pFixBro->right == NULL) || (pFixBro->right->color == RB_BLACK)) {
                    /* 此处pFixBro->left一定不为空 */
                    pFixBro->left->color = RB_BLACK;
                    pFixBro->color = RB_RED;
                    RbRightRotate(pFixBro, pTree);
                    pFixBro = pFixParent->right;
                }
                pFixBro->color = pFixParent->color;
                pFixParent->color = RB_BLACK;
                /* 此处pFixBro->right一定不为空 */
                pFixBro->right->color = RB_BLACK;
                RbLeftRotate(pFixParent, pTree);
                pNodeFix = pTree->root;
            }
        } else  {
            pFixBro = pFixParent->left;
            /* 此处pFixBro一定不为空 */
            if (pFixBro->color == RB_RED) {
                pFixBro->color = RB_BLACK;
                pFixParent->color = RB_RED;
                RbRightRotate(pFixParent, pTree);
                pFixBro = pFixParent->left;
            }
            if (((pFixBro->left == NULL) || (pFixBro->left->color == RB_BLACK)) &&
                ((pFixBro->right == NULL) || (pFixBro->right->color == RB_BLACK))) {
                pFixBro->color = RB_RED;
                pNodeFix = pFixParent;
                pFixParent = pFixParent->parent;
            } else {
                if ((pFixBro->left == NULL) || (pFixBro->left->color == RB_BLACK)) {
                    /* 此处pFixBro->left一定不为空 */
                    pFixBro->right->color = RB_BLACK;
                    pFixBro->color = RB_RED;
                    RbLeftRotate(pFixBro, pTree);
                    pFixBro = pFixParent->left;
                }
                pFixBro->color = pFixParent->color;
                pFixParent->color = RB_BLACK;
                /* 此处pFixBro->right一定不为空 */
                pFixBro->left->color = RB_BLACK;
                RbRightRotate(pFixParent, pTree);
                pNodeFix = pTree->root;
            }
        }
    }

    if (pNodeFix != NULL) {
        pNodeFix->color = RB_BLACK;
    }

    return;
}

/* 删除结点 */
void RbNodeDelete(RB_TREE_S *pTree, RB_NODE_S *pNodeRm)
{
    RB_NODE_S *pNodeFix;
    RB_NODE_S *pNodeTrans;
    RB_NODE_S *pFixParent = pNodeRm->parent;
    int oriColor = pNodeRm->color;

    if (pNodeRm->left == NULL) {
        pNodeFix = pNodeRm->right;
        RbNodeTransplant(pTree, pNodeRm, pNodeRm->right);
    } else if (pNodeRm->right == NULL) {
        pNodeFix = pNodeRm->left;
        RbNodeTransplant(pTree, pNodeRm, pNodeRm->left);
    } else {
        pNodeTrans = RbNodeGetMinimum(pNodeRm->right);
        /* 注意此处pNodeFix可能为空指针, pNodeTrans一定不为空 */
        pNodeFix = pNodeTrans->right;
        oriColor = pNodeTrans->color;
        if (pNodeTrans->parent != pNodeRm) {
            pFixParent = pNodeTrans->parent;
            RbNodeTransplant(pTree, pNodeTrans, pNodeTrans->right);
            pNodeTrans->right = pNodeRm->right;
            /* 此处pNodeRm的右子女一定不为空 */
            pNodeTrans->right->parent = pNodeTrans;
        } else {
            pFixParent = pNodeTrans;
        }
        RbNodeTransplant(pTree, pNodeRm, pNodeTrans);
        pNodeTrans->left = pNodeRm->left;
        pNodeTrans->left->parent = pNodeTrans;
        pNodeTrans->color = pNodeRm->color;
    }

    /* 释放删除结点内存 */
    RbNodeFree(pNodeRm);

    /* 恢复红黑树性质 */
    if (oriColor == RB_BLACK) {
        RbNodeDeleteFixUp(pTree, pNodeFix, pFixParent);
    }

    return;
}

/* 删除数据结点 */
int RbTreeRemove(RB_TREE_S *pRbTree, void *key)
{
    RB_NODE_S *pNode;

    /* 入参校验 */
    if ((pRbTree == NULL) || (pRbTree->cmpFunc == NULL) || (key == NULL)) {
        return RB_ERR;
    }

    pNode = RbNodeFind(pRbTree, key);
    if (pNode == NULL) {
        return RB_ERR;
    }

    RbNodeDelete(pRbTree, pNode);

    return RB_OK;
}

/* 销毁红黑树 */
void RbTreeDestroy(RB_TREE_S *pRbTree)
{
    return;
}

