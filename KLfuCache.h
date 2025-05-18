#pragma once

#include <cmath>
#include <memory>  //含有weak_ptr和shared
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "KICachePolicy.h"

namespace KamaCache
{
//循环依赖的解决
template<typename Key, typename Value> class KLfuCache;

template<typename Key, typename Value> 
class FreqList
{
private:
    struct Node
    {
        int freq;
        Key key;
        Value value;
        //对pre的weak_ptr的不增加引用计数,防止循环引用和内存泄漏
        std::weak_ptr<Node> pre;
        std::shared_ptr<node> next;
        
        Node(): freq(1), next(nullptr) {}
        Node(Key key, Value value)
        : freq(1), key(key), value(value), next(nullptr) {}
    };

    //type alias
    using NodePtr = std::shared_ptr<Node>;
    //成员变量,当前FreqList对应的访问频率
    int freq_;
    //假头尾节点
    NodePtr head_;
    NodePtr tail_;
    
public:
    //explicit防止被隐式转化调用
    explicit FreqList(int n)
    : freq_(n)
    {
        //make_shared创建了一个新的Node对象返回了一个shared_ptr指针相当于new
        head_ = std::make_shared<Node>();
        tail_ = std::make_shared<Node>();
        head_->next = tail_;
        tail_->pre = head_;
    }

    bool isEmpty() const
    {
        return head_->next == tail_;
    }

    //add node
    void addNode(NodePtr node)
    {
        if (!node || !head_ || !tail_)
            return;

        node->pre = tail_->pre;
        node_->next = tail;
        //pre是weak,用lock获得shared_ptr
        tail_->pre.lock()->next = node;
        tail_->pre = node;
    }
    
    void removeNode(NodePtr node) 
    {
        if (!node || !head_ || !tail_)
            return;
        if (node->pre.expired() || !node->next)
            return;
        
        auto pre = node->pre.lock();
        pre->next = node->next;
        node->next->pre = pre;
        //断开节点和链表
        node->next = nullptr;
    }

    NodePtr getFirstNode() const { return head_->next;}

    friend class KLfuCache<Key, Value>;
};

template<typename Key, typename Value>
class KLfuCache : public KICachePolicy<Key, Value>
{
    public:
        using Node = typename FreqList<Key, Value>::Node;







