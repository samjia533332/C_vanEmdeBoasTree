//
//  EVBTree.c
//  C_Emode-Van-Boas-Tree
//
//  Created by Zhenyu Jia on 2018/11/26.
//  Copyright © 2018年 Zhenyu Jia. All rights reserved.
//

#include "EVBTree.h"

struct node {
    UUU u;
    UUU subsize;
    
    UUU *min, *max;
    vEBNode *summary;
    vEBNode **cluster;
};

vEBNode*    createVEBTree       (UUU u) {
    vEBNode* ret;
    
    if ((ret = (vEBNode*) malloc(sizeof(vEBNode))) == NULL) return NULL;
    ret->u     = u;
    ret-> min  = NULL;
    ret-> max  = NULL;
    
    if (u == 2) {
        ret->summary  = NULL;
        ret->cluster  = NULL;
        ret->subsize  = 1;
    } else {
        UUU  clusterSize   = (UUU) sqrt(u);
        ret->subsize  = clusterSize;
        ret->summary  = (vEBNode*) malloc(sizeof(vEBNode));
        ret->cluster  = (vEBNode**) malloc(sizeof(vEBNode*) * clusterSize);
        for (int i = 0; i < clusterSize; i++) *(ret->cluster+i) = NULL;
    }
    return ret;
}

void        destroyVEBTree      (vEBNode* root) {
    if (!root) return;
    free(root->min);
    free(root->max);
    if (root->u != 2) {
        destroyVEBTree(root->summary);
        for (UUU i = 0; i < root->subsize; i++)
            destroyVEBTree(root->cluster[i]);
    }
    free(root);
}

/* ===== check member existence ===== */
// T(n) = T(sqrt(n)) + O(1)
bool        memberVEBTree       (vEBNode *root, UUU x) {
    if (!root->min) return false;
    if (*(root->min) == x || *(root->max) == x) return true;
    if (root->u == 2) return false;
    
    UUU  clusterSize   = (UUU) sqrt(root->u);
    UUU  high   = x / clusterSize;
    UUU  low    = x % clusterSize;
    if (!root->cluster[high]) return false;
    return memberVEBTree(root->cluster[high], low);
}

/* insert element into a tree, must control that x < u */
void        insertVEBTree       (vEBNode *root, UUU x) {
    // 如果超出范围 不能插入
    if (x >= root->u) {
        printf("Insert range out of VEBTree, insertion failed. \n");
        return;
    }
    // 当min 和 max 都消失时 直接更新 min/max O(1)
    if (!root->min) {
        root->min       = (UUU*) malloc(sizeof(UUU));
        root->max       = (UUU*) malloc(sizeof(UUU));
        *(root->max) = *(root->min) = x;
        return;
    }
    // 如果相同 表示 duplicate element 不能再插入
    if (x == *(root->min) || x == *(root->max)) return;
    
    if (root->u == 2) {
        if (x > *(root->max)) *(root->max) = x;
        if (x < *(root->min)) *(root->min) = x;
    } else {
        // 插入 比最小值小， 则需要把新的最小值更新 把这个值往下放
        // 对于最大最小来说 都可以直接放在上面 没有放在实际对应的格子里，这样其实就是加速
        if (x < *(root->min)) {
            UUU curMin      = *(root->min);
            *(root->min)    = x;
            insertVEBTree(root, curMin);
        } else if (x > *(root->max)) {
            // 原文代码实现里面并没有加入这个
            UUU curMax      = *(root->max);
            *(root->max)    = x;
            insertVEBTree(root, curMax);
        } else {
            UUU high        = x / root->subsize;
            UUU low         = x % root->subsize;
            if (!root->cluster[high]) {
                root->cluster[high]   = (vEBNode*) malloc(sizeof(vEBNode));
                // 这个是执行了 递归 call
                insertVEBTree(root->summary, high);
                // this O(1)  刚创建完插入 就是 O(1) 因为只更新了 min/max
                insertVEBTree(root->cluster[high], low);
            } else {
                insertVEBTree(root->cluster[high], low);
            }
        }
    }
}

UUU*        succVEBTree         (vEBNode *root, UUU x) {
    if (!root->min) return NULL;
    if (x >= *(root->max)) return NULL;
    if (x < *(root->min)) return root->min;
    
    // base case
    if (root->u == 2) {
        if (x == 0 && *(root->max) == 1) return root->max;
        return NULL;
    } else {
        UUU high  = x / root->subsize;
        UUU low   = x % root->subsize;
        UUU *jj, *ii;
        
        if (!root->cluster[high] || !root->cluster[high]->min)
            return NULL;
        // 如果 low 比 当前的最大值小的话 说明 succ 就在这个里面
        if (*(root->cluster[high]->max) > low) {
            ii     = &high;
            jj     = succVEBTree(root->cluster[high], low);
            if (!jj) return NULL;
        } else {
            ii     = succVEBTree(root->summary, high);
            if (!ii) return NULL;
            jj     = root->cluster[*ii]->min;
            if (!jj) return NULL;
        }
        UUU* ret = (UUU*) malloc(sizeof(UUU));
        *ret     = (root->subsize) * (*ii) + (*jj);
        return ret;
    }
}

UUU*        predVEBTree         (vEBNode *root, UUU x) {
    if (!root->min) return NULL;
    if (*(root->min) >= x) return NULL;
    if (*(root->max) < x) return root->max;
    
    // base case
    if (root->u == 2) {
        if (x == 1 && *(root->min) == 0) return root->min;
        return NULL;
    } else {
        UUU high     = x / root->subsize;
        UUU low      = x % root->subsize;
        UUU *ii, *jj;
        
        if (!root->cluster[high] || !root->cluster[high]->min)
            return NULL;
        // 如果 low 比 当前的最小大的话 说明 pred 就在这个里面
        if (*(root->cluster[high]->min) < low)  {
            jj       = predVEBTree(root->cluster[high], low);
            ii       = &high;
        } else {
            ii       = predVEBTree(root->summary, high);
            if (!ii) return NULL;
            jj       = root->cluster[*ii]->max;
            if (!jj) return NULL;
        }
        UUU *ret     = (UUU*) malloc(sizeof(UUU));
        *ret         = root->subsize * (*ii) + (*jj);
        return ret;
    }
}

void        deleteVEBTree       (vEBNode *root, UUU x) {
    // 超过值域了
    if (x >= root->u) return;
    // 当前node 是空的无法删除
    if (!root->min) return;
    // 如果当前结点只有一个值 那么直接删掉
    if (x == *(root->min) && x == *(root->max)) {
        free(root->max);
        free(root->min);
        root->max = root->min = NULL;  // summary should be empty
        return;
    }
    // 如果是最小值要删除 需要再拿一个最小值上来
    if (x == *(root->min)) {
        UUU* i = root->summary->min;
        if (!i) {
            // 表示这个子机构里面没有了 那么 max 直接就是最小值了 因为max 和min 不相等
            *root->min = *root->max;
            return;
        }
        *(root->min)   = root->subsize * (*i) + *(root->cluster[*i]->min);  // 不能把指针传给上面 要把值贴过来因为后面那个要释放。
        x              = *(root->min);
    }
    // 这个在原有的代码里面不是这样的
    
    if (x == *(root->max)) {
        UUU* i = root->summary->max;
        if (!i) {
            // 如果 子结构没有了 那么删除max 以后 min 就是max
            *root->max = *root->min;
            return;
        }
        *(root->max)  = *(i) * root->subsize + *(root->cluster[*i]->max);
        x             = *(root->max);
    }
    
    UUU   high     = x / root->subsize;
    UUU   low      = x % root->subsize;
    deleteVEBTree(root->cluster[high], low);
    if (!root->cluster[high]->min)
        deleteVEBTree(root->summary, high);
}
