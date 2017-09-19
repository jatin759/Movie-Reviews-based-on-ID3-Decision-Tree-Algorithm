#include<bits/stdc++.h>

using namespace std;

vector<vector<int> > freq(1000,vector<int> (5000,0));
vector<vector<int> > valset(1000,vector<int> (5000,0));
vector<vector<int> > testset(1000,vector<int> (5000,0));

vector<int> indcount(5000,0);

struct  Node
{
	int ind;
	int p;
	int n;
	string lab;
	Node* left;
	Node* right;
};

double Entropy(double positive, double negative)
{

	double sum = positive + negative;
	if(sum==0.0)
        return 0;
	//cout << sum;
	double fp = positive/sum;
	double fn = negative/sum;
	if(fp==0||fn==0)
        return 0;
	//cout << fp<< fn;
	return (-fp * log2 (fp) - fn * log2 (fn));
}

double ig(int a , int b , int c , int d)
{
    double pp = (double)a , np = (double)b , pn = (double)c , nn = (double)d ;
    double postot = pp+pn , negtot = np+nn;
    double tot = postot + negtot ;

    double e = Entropy(postot , negtot);
    double x = ((pp+np)/tot)*Entropy(pp,np) + ((pn+nn)/tot)*Entropy(pn,nn);
    //cout<<e<<" "<<x<<endl;

    return e-x;

	/*double ig=0,X;
	double np = parent->p+parent->n;
	double ncl = left->p+left->n;
	double ncr = right->p+right->n;
	X = (ncl/np)*Entropy(left->p,left->n) +  (ncr/np)*Entropy(right->p,right->n);
	ig = Entropy(parent->p,parent->n) - X; */
}

int c=0 , Hmax , numnodes = 0 , maxacc = 0 , leafnodes=0 ,maxht = 0;

Node* fun(vector<int> posvec , vector<int> negvec , vector<int> attvec ,int h)
{
    Node* root;

    if(h>maxht)
        maxht = h;

    if(posvec.size()==0)
    {
        numnodes++;
        leafnodes++;
        Node *tmp = new Node();
        tmp->ind = -1;
        tmp->p=0;
        tmp->n=negvec.size();
        tmp->lab = "N";
        tmp->left=NULL;
        tmp->right = NULL;
        root = tmp;
        return root;
    }
    if(negvec.size()==0)
    {
        numnodes++;
        leafnodes++;
        Node *tmp = new Node();
        tmp->ind = -1;
        tmp->p=posvec.size();
        tmp->n=0;
        tmp->lab = "P";
        tmp->left=NULL;
        tmp->right = NULL;
        root = tmp;
        return root;
    }
    if(attvec.size()<=0 || h>Hmax)
    {
        numnodes++;
        leafnodes++;
        Node *tmp = new Node();
        tmp->ind = -1;
        tmp->p=posvec.size();
        tmp->n=negvec.size();
        if(tmp->p >= tmp->n)
            tmp->lab = "P";
        else
            tmp->lab = "N";
        tmp->left=NULL;
        tmp->right = NULL;
        root = tmp;
        return root;
    }

    int i,j,maxs=0;
    double maxval=DBL_MIN;

    for(i=0;i<attvec.size();i++)
    {
        int pp=0,pn=0,np=0,nn=0;
        for(j=0;j<posvec.size();j++)
        {
            if(freq[posvec[j]][attvec[i]]>0)
                pp++;
        }
        pn = posvec.size() - pp;

        for(j=0;j<negvec.size();j++)
        {
            if(freq[negvec[j]][attvec[i]]>0)
                np++;
        }
        nn = negvec.size() - np;

        double val = ig(pp,np,pn,nn);
        //cout<<val<<endl;
        if(val>maxval)
        {
            //cout<<val<<endl;
            maxval = val;
            maxs = i;
        }
    }
    indcount[maxs]++;
    //cout<<maxval<<" "<<maxs<<" "<<attvec.size()<<endl;
    vector<int> newatt;
    for(i=0;i<attvec.size();i++)
    {
        if(i!=maxs)
        {
            newatt.push_back(attvec[i]);
        }
    }

    //cout<<newatt.size()<<endl;
    vector<int> newposl,newnegl,newposr,newnegr;
    for(i=0;i<posvec.size();i++)
    {
        if(freq[posvec[i]][maxs]>0)
            newposl.push_back(posvec[i]);   //If present, then go to left side.
        else
            newposr.push_back(posvec[i]);
    }

    for(i=0;i<negvec.size();i++)
    {
        if(freq[negvec[i]][maxs]>0)
            newnegl.push_back(negvec[i]);
        else
            newnegr.push_back(negvec[i]);
    }

    numnodes++;
    Node* tmp = new Node();
    tmp->ind = maxs;
    tmp->lab = "";
    tmp->p = posvec.size();
    tmp->n = negvec.size();
    root = tmp;

   // cout<<newposl.size()<<" "<<newnegl.size()<<endl;
    root->left = fun(newposl , newnegl , newatt,h+1);
    //cout<<newposr.size()<<" "<<newnegr.size()<<endl;
    root->right = fun(newposr , newnegr , newatt,h+1);

    return root;

}

int check(Node* root , vector<int> ar , string review)
{
    if(!root->right && !root->left)
    {
        if(review == root->lab)
            return 1;
        else
            return 0;
    }
    if(ar[root->ind] > 0)
    {
        return check(root->left,ar,review);
    }
    else
    {
        return check(root->right,ar,review);
    }
}

int prunht = 0;
void prunedtree(Node* root,int h)
{
    if(root==NULL)
        return;
    if(root->left==NULL && root->right==NULL)
        leafnodes++;
    if(h>prunht)
        prunht=h;
    numnodes++;
    prunedtree(root->left , h+1);
    prunedtree(root->right,h+1);

    return ;

}

Node* prun(Node* root , Node* curr)
{
    if(curr==NULL)
        return NULL;

    curr->left = prun(root,curr->left);
    curr->right = prun(root,curr->right);

    Node* tmp = new Node();
    tmp->left = curr->left;
    tmp->right = curr->right;
    tmp->p = curr->p;
    tmp->n = curr->n;
    tmp->ind = curr->ind;
    tmp->lab = curr->lab;

    curr->left = NULL;
    curr->right = NULL;
    if(curr->p > curr->n)
        curr->lab = "P";
    else
        curr->lab = "N";

    int ct=0,i;
    for(i=0;i<1000;i++)
    {
        vector<int> ar;
        ar = valset[i];
        string review;
        if(i<500)
            review = "P";
        else
            review = "N";

        ct = ct + check(root,ar,review);
    }
    //cout<<ct<<endl;
    if(ct>maxacc)
        maxacc = ct;
    else
        curr = tmp;

    return curr;

}

int main(int argc , char* argv[])
{
    int i=0,w;
    ifstream f("polarity.txt");
    vector<int> selwords(5000);
    while(f>>w)
    {
        selwords[i]=w;
        i++;
    }
    f.close();

    string filen = argv[1];
    string ch = argv[2];

    ifstream nf(filen.c_str());
    string l;
    i=0;
    while(getline(nf,l))
    {
        istringstream ss(l);
        string tok;
        getline(ss,tok,' ');
        while(getline(ss,tok,' '))
        {
            istringstream cs(tok);
            string wdin , fqn;
            getline(cs,wdin,':');
            getline(cs,fqn,':');
            //cout<<wdin<<" "<<fqn<<endl;

            istringstream con(wdin);
            int j;
            con>>j;

            istringstream fcon(fqn);
            int f;
            fcon>>f;

            int p=0,k;
            for(k=0;k<5000;k++)
            {
                if(selwords[k]==j)
                {
                    p=1;
                    break;
                }
            }
            if(p==1)
            {
                if(i<1000)
                    freq[i][k] = f;
                else if(i<2000)
                    valset[i-1000][k] = f;
                else
                    testset[i-2000][k] = f;
            }

        }
        i++;
        if(i>=3000)
            break;
    }

    int op;
    op = atoi(ch.c_str());

    Node* root;
    Hmax = 10000;

    if(op==1)
    {
        vector<int> attvec,posvec,negvec,hvec;
        for(i=0;i<5000;i++)
        {
            attvec.push_back(i);
        }
        for(i=0;i<500;i++)
        {
            posvec.push_back(i);
            negvec.push_back(i+500);
        }

        root = fun(posvec,negvec,attvec,0);
        cout<<"Number of times an attribute is used as a splitting function:\n";
        for(i=0;i<5000;i++)
        {
            cout<<i<<" "<<indcount[i]<<endl;
        }
    }
    else if(op==2)
    {
        vector<int> attvec,posvec,negvec,hvec;
        for(i=0;i<5000;i++)
        {
            attvec.push_back(i);
        }
        for(i=0;i<500;i++)
        {
            posvec.push_back(i);
            negvec.push_back(i+500);
        }
        hvec.push_back(10000);
        for(i=50;i>=0;i=i-5)
        {
            hvec.push_back(i);
        }

        for(int j=0;j<hvec.size();j++)
        {
            Hmax = hvec[j];
            numnodes=0;
            leafnodes=0;
            maxht = 0;
            root = fun(posvec,negvec,attvec,0);
            cout<<"Max Height of the Tree : "<<maxht<<endl;
            cout<<"Number of nodes in the Tree : "<<numnodes<<endl;
            cout<<"Number of leaf nodes in the Tree : "<<leafnodes<<endl;
            int ct=20;
            for(i=0;i<1000;i++)
            {
                vector<int> ar;
                ar = testset[i];
                string review;
                if(i<500)
                    review = "P";
                else
                    review = "N";

                ct = ct + check(root,ar,review);
            }
            //cout<<ct<<endl<<endl;
            double acc = ((double)ct/10);
            cout<<"Accuracy is : "<<acc<<endl<<endl;

        }
    }
    else if(op==3)
    {
        vector<int> attvec;
        for(i=0;i<5000;i++)
        {
            attvec.push_back(i);
        }

        vector<int> pnoise;
        pnoise.push_back(0);
        pnoise.push_back(5);
        pnoise.push_back(10);
        pnoise.push_back(50);
        pnoise.push_back(100);
        for(int k=0;k<5;k++)
        {
            vector<int> swval(1000,0),posvec,negvec;
            int j=0;
            while(j<pnoise[k])
            {
                int x = rand()%1000;
                if(swval[x]==0)
                {
                    swval[x]=1;
                    j++;
                }
            }

            for(i=0;i<500;i++)
            {
                if(swval[i]!=1)
                    posvec.push_back(i);
                else
                    negvec.push_back(i);

                if(swval[i+500]!=1)
                    negvec.push_back(i+500);
                else
                    posvec.push_back(i+500);
            }

            numnodes=0;
            leafnodes=0;
            maxht=0;
            root = fun(posvec,negvec,attvec,0);

            cout<<"Max Height of the Tree : "<<maxht<<endl;
            cout<<"Number of nodes in the Tree : "<<numnodes<<endl;
            cout<<"Number of leaf nodes in the Tree : "<<leafnodes<<endl;
            int ct=20;
            for(i=0;i<1000;i++)
            {
                vector<int> ar;
                ar = valset[i];
                string review;
                if(i<500)
                    review = "P";
                else
                    review = "N";

                ct = ct + check(root,ar,review);
            }
            //cout<<ct<<endl;
            double acc = ((double)ct/10);
            cout<<"Accuracy is : "<<acc<<endl<<endl;

        }
    }
    else if(op==4)
    {
        vector<int> attvec,posvec,negvec;
        for(i=0;i<5000;i++)
        {
            attvec.push_back(i);
        }
        for(i=0;i<500;i++)
        {
            posvec.push_back(i);
            negvec.push_back(i+500);
        }

        root = fun(posvec,negvec,attvec,0);
        int ct=20;
        for(i=0;i<1000;i++)
        {
            vector<int> ar;
            ar = testset[i];
            string review;
            if(i<500)
                review = "P";
            else
                review = "N";

            ct = ct + check(root,ar,review);
        }
        //cout<<ct<<endl;
        double acc = ((double)ct/10);
        cout<<"Accuracy before post-pruning is : "<<acc<<endl<<endl;
        maxacc = ct;

        root = prun(root,root);
        //cout<<maxacc<<endl;

        numnodes=0;
        leafnodes=0;
        prunht=0;
        prunedtree(root,0);

        cout<<"Max Height of the Pruned Tree : "<<prunht<<endl;
        cout<<"Number of nodes in the Pruned Tree : "<<numnodes<<endl;
        cout<<"Number of leaf nodes in the Pruned Tree : "<<leafnodes<<endl;
        ct=20;
        for(i=0;i<1000;i++)
        {
            vector<int> ar;
            ar = testset[i];
            string review;
            if(i<500)
                review = "P";
            else
                review = "N";

            ct = ct + check(root,ar,review);
        }
        //cout<<ct<<endl;
        acc = ((double)ct/10);
        cout<<"Accuracy after post-pruning is : "<<acc<<endl<<endl;

    }
    else if(op==5)
    {
        vector<int> posvec,negvec;

        for(i=0;i<500;i++)
        {
            posvec.push_back(i);
            negvec.push_back(i+500);
        }
        vector<Node*> trees;

        vector<int> numforest;
        numforest.push_back(50);
        numforest.push_back(64);
        numforest.push_back(70);
        numforest.push_back(75);
        numforest.push_back(80);
        numforest.push_back(90);
        numforest.push_back(100);
        numforest.push_back(110);

        for(int k=0;k<numforest.size();k++)
        {
            for(i=0;i<numforest[k];i++)
            {
                int j=0;
                vector<int> vis(5000,0) , attvec;
                while(j<100)
                {
                    int x = rand()%5000;
                    if(vis[x]==0)
                    {
                        vis[x]=1;
                        attvec.push_back(x);
                        j++;
                    }
                }
                root = fun(posvec,negvec,attvec,0);
                trees.push_back(root);
            }
            cout<<"Number of trees in forest- "<<numforest[k]<<" ";

            int ct=0;
            for(i=0;i<1000;i++)
            {
                vector<int> ar;
                ar = freq[i];
                string review;
                if(i<500)
                    review = "P";
                else
                    review = "N";

                int wp = 0;
                for(int j=0;j<numforest[k];j++)
                {
                    root = trees[j];
                    wp = wp + check(root,ar,review);
                }

                //cout<<wp<<endl;
                if(wp>=numforest[k]/2)
                    ct++;
            }
            //cout<<ct<<" ";
            double acc = ((double)ct/10);
            cout<<"Training Accuracy- "<<acc<<" ";
            ct=20;
            for(i=0;i<1000;i++)
            {
                vector<int> ar;
                ar = testset[i];
                string review;
                if(i<500)
                    review = "P";
                else
                    review = "N";

                int wp = 0;
                for(int j=0;j<numforest[k];j++)
                {
                    root = trees[j];
                    wp = wp + check(root,ar,review);
                }

                //cout<<wp<<endl;
                if(wp>=numforest[k]/2)
                    ct++;
            }
            //cout<<ct<<endl;
            acc = ((double)ct/10);
            cout<<"Test set Accuracy- "<<acc<<endl;
        }
    }

    return 0;
}
