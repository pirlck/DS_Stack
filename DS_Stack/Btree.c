//*******************************************
//B-trees.cpp   2015/9/27 by PirLCK
//֮ǰ��B������һ�£����ֲ�̫�ѣ���������
//��д��B������ȴ���������ˣ����ǵðѴ���дд
//*******************************************
#include<iostream>
#include <deque>
template<typename Type>
class b_trees{
private:
    //���Ķ���
    struct node{
        node(){
            leaf=true;
        }
        std::deque<Type> keys;
        std::deque<node*> children;
        bool leaf;
    };
public:
    //����һ�ſ���(���캯��)
    explicit b_trees(int deg=10):root(new node),degree(deg){};
    ~b_trees(){   delete_tree(root); };
    //���Ҳ���
    std::pair<node*, int> search(Type key)
    {
        node *pnode=root;
        while(true){
            auto iter=pnode->keys.begin();
            while (iter != pnode->keys.end() && *iter < key)
                ++iter;
            int i=iter- pnode->keys.begin();
            if(iter != pnode->keys.end() && key== *iter )
                return std::make_pair(pnode,i);
            else{
                if(pnode->leaf){
                    return std::make_pair(nullptr,0);
                }
                else
                    pnode=pnode->children[i];
            }
        }
    }
    //���������
    node* insert(Type key){
        node*  pnode=root;
        if (pnode->keys.size() == 2*degree-1){
            root=new node;
            root->leaf=false;
            root->children.push_back(pnode);
            splite_child(root, 0);
            pnode=root;
        }
        while(! pnode->leaf){
            int    i=0;
            while (i != pnode->keys.size() && pnode->keys[i] < key)
                ++i;
            if (pnode->children[i]->keys.size() == 2*degree-1)
            {
                splite_child(pnode,i);
                if(pnode->keys[i] < key)
                    ++i;
            }
            pnode=pnode->children[i];
        }
        auto iter=pnode->keys.begin(); 
        while(iter != pnode->keys.end() && key > *iter)
            ++iter;
        pnode->keys.insert(iter, key);
        return pnode;
    }
    // ɾ������
    bool erase(Type key){
        node* pnode=root;
        int i=0;
        //step1
        while( !pnode->leaf  ){
            auto iter=pnode->keys.begin();
            while (iter != pnode->keys.end() && *iter < key)
                ++iter;
            i=iter- pnode->keys.begin();
            if(iter != pnode->keys.end() && *iter == key)
                break;
            else
            {
                if(pnode->children[i]->keys.size() == degree-1)
                    pnode=grow_keys(pnode, i);
                else
                    pnode=pnode->children[i];
            }
        };
        //step2
        if( !pnode-> leaf){
            return inter_erase(pnode, i);
        }
        //step3
        else{
            auto iter = pnode->keys.begin();
            while (iter != pnode->keys.end() && *iter < key)
                ++iter;
            if(iter == pnode->keys.end() || *iter != key )
                return false;
            else{
                pnode->keys.erase(iter);
                return true;
            }
        }
    }
private:
    void splite_child(node* pnode,  int i){
        //�ú�������x�ĺ������±�Ϊi�ĺ���
        node *x=pnode->children[i];
        node* new_node=new node;
        new_node->leaf=x->leaf;
        new_node->keys.insert(new_node->keys.begin(), 
            x->keys.begin()+degree, x->keys.end());
        Type key=*(x->keys.begin()+degree-1);
        x->keys.erase(x->keys.begin()+degree-1, x->keys.end());
        if(! x->leaf){
            new_node->children.insert(new_node->children.begin(),
                x->children.begin()+degree, x->children.end() );
            x->children.erase(
                x->children.begin()+degree,x->children.end());
        }
        pnode->keys.insert(pnode->keys.begin()+i, key );
        pnode->children.insert(pnode->children.begin()+i+1, new_node);
    }
    node* grow_keys(node* pnode,  int i){
        //*�ú�������pnode���±�Ϊi�ĺ��ӽ���degree-1���ؼ���
        //*����ִ�к�֤�Ǹ�����������degree���ؼ��֣������ش����Ľ�㡣
        //����еĻ���������ڵ��ֵܽ�һ���ؼ��֡�
        if(i>0 && pnode->children[i-1]->keys.size() > degree-1){
            Type tmp_key = pnode->children[i-1]->keys.back();
            pnode->children[i-1]->keys.pop_back();
            pnode->children[i]->keys.push_front(pnode->keys[i]);
            pnode->keys[i] = tmp_key;
            //���ú���صĺ���ָ��
            if( ! pnode->children[i-1]->leaf){
                node *tmp_node= pnode->children[i-1]->children.back();
                pnode->children[i-1]->children.pop_back();
                pnode->children[i]->children.push_front(tmp_node);
            }
            return pnode->children[i];
        }
        //�������ֵ�û�У���ô���ұ����ڵ��ֵܽ�
        else {
            if(i< pnode->keys.size () 
                && pnode->children[i+1]->keys.size() > degree-1 )
            {
                Type tmp_key = pnode->children[i+1]->keys.front();
                pnode->children[i+1]->keys.pop_front();
                pnode->children[i]->keys.push_back(pnode->keys[i]);
                pnode->keys[i] = tmp_key;
                //���ú���صĺ���ָ��
                if( ! pnode->children[i+1]->leaf){
                    node *tmp_node= pnode->children[i+1]->children.front();
                    pnode->children[i+1]->children.pop_front();
                    pnode->children[i]->children.push_back(tmp_node);
                }
                return pnode->children[i];
            }
            else{
                //�����������������ֵܶ��޹ؼ��ֿɽ裬��ôҪ���кϲ���������
                if(i!=0)
                    return merge_node(pnode, i-1);
                else
                    return merge_node(pnode, i);
            }
        }
    }
    node* merge_node(node* pnode, int i){
        //�ú����ϲ�pnode-keys[i] �������ߵĵ������ӽڵ㡣
        //�ú��������κ���֤���뱣֤����Ҫ�ϲ��Ľ��Ĺؼ�����Ŀȷʵ�� degree-1
        node* target=pnode->children[i];
        target->keys.push_back(pnode->keys[i]);
        target->keys.insert(
            target->keys.end(), 
            pnode->children[i+1]->keys.begin(),
            pnode->children[i+1]->keys.end());
        if(! target->leaf){
            target->children.insert(
                target->children.end(), 
                pnode->children[i+1]->children.begin(), 
                pnode->children[i+1]->children.end());
        }
        delete pnode->children[i+1];
        pnode->keys.erase(pnode->keys.begin()+i);
        pnode->children.erase(pnode->children.begin()+i+1);
        node* result=pnode->children[i];
        if(pnode==root && pnode->keys.size()==0)
        {
            root=pnode->children[0];
            delete pnode;
            result = root;
        }
        return result;
    }
    bool inter_erase(node* pnode, int i){
        if(pnode->leaf){
            pnode->keys.erase(pnode->keys.begin()+i);
            return true;
        }
        else{
            if( pnode->children[i]->keys.size() > degree-1){
                //case1: ���ӹؼ��ָ���Ļ��ݹ��½�������
                pnode->keys[i]= pnode->children[i]->keys.back();
                inter_erase(
                    pnode->children[i], pnode->children[i]->keys.size()-1);
            }
            else{
                if( pnode->children[i+1]->keys.size() > degree-1){
                    //case1: �Һ��ӹؼ��ָ���Ļ��ݹ��½�������
                    pnode->keys[i]= pnode->children[i+1]->keys.front();
                    inter_erase(pnode->children[i+1], 
                        pnode->children[i+1]->keys.size()-1);
                }
                else{//case3: ���Һ��ӹؼ��ֶ������࣬��ô�����ߺϲ���·��
                    pnode = merge_node(pnode, i);
                    inter_erase(pnode, degree-1);
                }
            }
        }
    }
    void delete_tree(node *x){
        if(x->leaf){
            delete x;
            x=NULL;
            return;
        }
        else{
            for(auto iter= x->children.begin();
                iter!= x->children.end(); ++iter)
            {
                delete_tree(*iter);
            }
        }
    }
private:
    node* root;
    int degree;
};