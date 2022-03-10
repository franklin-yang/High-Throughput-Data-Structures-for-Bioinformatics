#include <sdsl/suffix_arrays.hpp>
#include <fstream>
/* printf */
#include <math.h>
#include <vector>
#include "../RankSupport/RankSupport.cpp"
// using namespace sdsl; 

class SelectSupport
{
public:
    RankSupport *rs;
    SelectSupport(RankSupport *ranksupport)
    {
        rs = ranksupport;
    }

    uint64_t select1(uint64_t i)
    {

        int size = rs->bv->size();
        int lIdx = 0;
        int rIdx = size - 1;
        int mIdx = (lIdx + rIdx) / 2;
        int mRank = rs->rank1(mIdx);

        // for (int i = 0; i < rs->bv->size(); i++)
        // {
        //     std::cout << (*(rs->bv))[i] << "b" << rs->rank1(i) << " ";
        // }
        // std::cout << "midx" << mIdx << "r" << rs->rank1(mIdx) << std::endl;
        while (mRank != i || (*(rs->bv))[mIdx] == 0)
        {
            // std::cout << "mRank:" << mIdx << std::endl;
            // std::cout << "target rank:" << i << " lidx:" << lIdx << " ridx:" << rIdx << "mrank" <<mRank<< "midx" << mIdx <<  std::endl;
            // print_bit_vec(rs->bv);
            if (mRank > i)
            {
                rIdx = mIdx - 1;
            }
            else if (mRank < i)
            {
                lIdx = mIdx + 1;
            }
            else
            {
                rIdx = mIdx - 1;
            }
            mIdx = (rIdx + lIdx) / 2;
            mRank = rs->rank1(mIdx);
        }
        return mIdx;
    }

    uint64_t overhead()
    {
        return rs->overhead();
    }

    void load(std::string &fname)
    {
        rs->load(fname);
    }

    void save(std::string &fname)
    {
        rs->save(fname);
    }
};

void get_space_data_t2()
{
    std::ofstream myfile;
    myfile.open("t2space.csv");
    // const int num_ops = 1000000;
    // int n = 1000;
    // int idxs[num_ops] ;
    myfile << "N,space\n";
    for (int s = 1; s < 25; s += 1)
    {
        std::cout << "make bv on " << s << std::endl
                  << std::flush;

        bit_vector b1 = bit_vector(pow(2, s), 0);
        for (int i = 0; i < pow(2, s); i++)
        {
            // std::cout << i << std::flush;
            if (rand() > RAND_MAX / 2)
                (b1)[i] = 1;
            // std::cout << 3 ;
        }
        RankSupport rs1 = RankSupport(&b1);
        std::cout << "ranked" << std::endl
                  << std::flush;
        SelectSupport ss = SelectSupport(&rs1);
        // rank_support_v<> rs1s = rank_support_v(b1);
        // double elapsed = (double)( end-start);
        // std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()<< std::endl;
        myfile << pow(2, s) << "," << ss.overhead() << "\n";
    }
    myfile.close();
}


bool test_select_save_load()
{
    for (int s = 1; s < 25; s += 1)
    {
        std::cout << "make bv on " << s << std::endl
                  << std::flush;

        int size = pow(2, s);
        bit_vector b1 = bit_vector(size, 0);
        random_populate(b1);
        RankSupport rs1 = RankSupport(&b1);
        SelectSupport ss1 = SelectSupport(&rs1);
        std::string fname = "temp";
        ss1.save(fname);
        
        bit_vector b2 = bit_vector(size, 0);
        RankSupport rs2 = RankSupport(&b2);
        SelectSupport ss2 = SelectSupport(&rs2);
        ss2.load(fname);
        
        for (int i = 1; i < rs2.rank1(size-1); i++)
        {
            int ex = ss1.select1(i);
            int got = ss2.select1(i);
            if (got != ex)
            {
                std::cout << "r" << i << " got:" << got << " ex:" << ex << std::endl;
                return false;
            }
        }
        std::cout << s << " complete " << std::endl
                  << std::flush;
    }
    return true;
}

class SparseArray
{

public:
    bit_vector bv;
    rank_support_v<1> rs;
    std::vector<std::string> elements;
    void create(uint64_t size)
    {
        std::cout << "cre" << std::endl
                  << std::flush;
        bv = bit_vector(size, 0);
        std::cout << "bv" << std::endl
                  << std::flush;
        util::init_support(rs, &bv);
        // rs = RankSupport(&bv);
    }

    void append(std::string elem, uint64_t pos)
    {
        elements.push_back(elem);
        bv[pos] = 1;
        util::init_support(rs, &bv);
    }

    bool get_at_rank(uint64_t r, std::string &elem)
    {
        if (r < elements.size())
        {
            elem = elements[r];
            return true;
        }
        else
        {
            return false;
        }
    }
    bool get_at_index(uint64_t r, std::string &elem)
    {
        if (bv[r])
        {
            int rank = rs.rank(r);
            elem = elements[rank];
            return true;
        }
        else
        {
            return false;
        }
    }
    uint64_t num_elem_at(uint64_t r)
    {
        return rs.rank(r);
    }
    uint64_t size()
    {
        RankSupport r = RankSupport(&bv);
        return (8 * sizeof(std::string) * num_elem()) + r.overhead();
    }
    uint64_t num_elem()
    {
        return elements.size();
    }
    void load(std::string& fname){
        std::ifstream seqOut(fname);
        bv.load(seqOut);
        util::init_support(rs,&bv);
        std::string line;
        elements.clear();
        while(getline(seqOut, line)){
            elements.push_back(line);
        }
        seqOut.close();
    }
    void save(std::string& fname){
        std::ofstream seqOut(fname);
        bv.serialize(seqOut);
        for(int i=0;i<elements.size();++i){
            seqOut<<elements[i]<<std::endl;
        }
        seqOut.close();
        // uint64_t ne = num_elem();
        // seqOut.write((const char*)&ne, sizeof(uint64_t));
        
        // seqOut << num_elem();
        // for
    }
};

bool test_select()
{
    for (int s = 30; s < 99999999999999; s += 1)
    {
        std::cout << "make bv on " << s << std::endl
                  << std::flush;

        int size = pow(2, s);
        bit_vector b1 = bit_vector(size, 0);
        random_populate(b1);
        RankSupport rs1 = RankSupport(&b1);
        int num1s = rs1.rank1(b1.size() - 1);

        SelectSupport ss1 = SelectSupport(&rs1);
        rank_support_v<> rs1s = rank_support_v(&b1);
        std::cout << "num1s" << num1s << "check" << rs1s.rank(b1.size()) << std::endl
                  << std::flush;
        // std::cout << "num1s" << num1s << "check" << rs1s.rank(b1.size()) << std::endl << std::flush;

        // print_bit_vec(&b1);
        select_support_mcl<> ss1s = select_support_mcl(&b1);
        int rs[num1s];
        for (int i = 1; i < num1s + 1; i++)
        {
            int got = ss1.select1(i);
            int ex = ss1s.select(i);
            if (got != ex)
            {
                std::cout << "r" << i << " got:" << got << " ex:" << ex << std::endl;
                return false;
            }
            else
            {
                // std::cout << "r" << i << " is:" << got << " ex:" <<ex << std::endl;
            }
        }
        // for (int i = 1; i < b1.size(); i++)
        // {
        //     if (ss1.select1(i) != ss1s.select(i))
        //     {
        //         std::cout << "i" << i << " got:" << ss1.select1(i) << " ex:" << ss1s.select(i) << std::endl;
        //         return false;
        //     }
        // }
        std::cout << s << " complete " << std::endl
                  << std::flush;
    }
}

// code below comes from geeksforgeeks
// https://www.geeksforgeeks.org/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/
// A utility function to swap to integers
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// // A utility function to print an array
// void printArray (int arr[], int n)
// {
//     for (int i = 0; i < n; i++)
//         cout << arr[i] << " ";
//     cout << "\n";
// }

// A function to generate a random
// permutation of arr[]
void randomize(int arr[], int n)
{
    // Use a different seed value so that
    // we don't get same result each time
    // we run this program
    srand(time(NULL));

    // Start from the last element and swap
    // one by one. We don't need to run for
    // the first element that's why i > 0
    for (int i = n - 1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        int j = rand() % (i + 1);

        // Swap arr[i] with the element
        // at random index
        swap(&arr[i], &arr[j]);
    }
}
void t3(){
    std::ofstream myfile;
    myfile.open("t3.csv");
    myfile << "size"
           << ","
           << "percent filled"
           << ","
           << "get at rank"
           << ","
           << "get at index"
           << ","
           << "num elem at"
           << ","
           << "size runtime"
           << ","
           << "num elem"
           << ","
           << "space used"
           << "\n";

    int sizes[] = {1000, 10000, 100000, 1000000};
    double percents[] = {0.01, .05, .1};
    int num_gets = 1000;
    for (int size : sizes)
    {
        std::cout << size << std::endl
                  << std::flush;
        int nums_to_insert[4];
        for (int i = 0; i < 3; i++)
        {
            nums_to_insert[i] = (int)(size * percents[i]);
            SparseArray sa;
            sa.create(size);
            int num_to_insert = (int)(size * percents[i]);
            int idxs[size];
            for (int j = 0; j < size; j++)
            {
                idxs[j] = j;
            }
            // int idxs_to_ins[num_to_insert];
            randomize(idxs, size);
            std::sort(idxs, idxs + num_to_insert);
            for (int j = 0; j < num_to_insert; j++)
            {
                sa.append("test", idxs[j]);
            }
            std::string dest;
            
            int get_ranks[num_gets];
            for (int j = 0; j < num_gets; j++)
            {
                get_ranks[j] = (rand() % num_to_insert);
            }
            auto start = std::chrono::steady_clock::now();
            
            for (int j = 0; j < num_gets; j++)
            {
                sa.get_at_rank(get_ranks[j], dest);
            }

            auto end = std::chrono::steady_clock::now();
            long long gar_t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            
            int get_idxs[num_gets];
            for (int j = 0; j < num_gets; j++)
            {
                get_idxs[j] = (rand() % size);
            }
            start = std::chrono::steady_clock::now();
            for (int j = 0; j < num_gets; j++)
            {
                sa.get_at_index(get_idxs[j], dest);
            }
            end = std::chrono::steady_clock::now();
            long long gai_t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            start = std::chrono::steady_clock::now();
            for (int j = 0; j < num_gets; j++)
            {
                sa.num_elem_at(get_idxs[j]);
            }
            end = std::chrono::steady_clock::now();
            long long nea_t = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            start = std::chrono::steady_clock::now();
            for (int j = 0; j < num_gets; j++)
            {
                sa.size();
            }
            end = std::chrono::steady_clock::now();
            long long sizet = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            start = std::chrono::steady_clock::now();
            for (int j = 0; j < num_gets; j++)
            {
                sa.num_elem();
            }
            end = std::chrono::steady_clock::now();
            long long ngt = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            // std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()<< std::endl;
            myfile << size << "," << percents[i] << "," << gar_t << "," << gai_t << "," << nea_t << "," << sizet << "," << ngt << "," << sa.size() << "," << (32 * size) + RankSupport(&sa.bv).overhead() << "\n";
        }
    }
}

// void test_sa

void get_performance_data_t2()
{
    std::ofstream myfile;
    myfile.open("t2time.csv");
    const int num_ops = 1000000;
    int ranks[num_ops];
    myfile << "N,time for " << num_ops << " operations\n";
    for (int s = 1; s < 25; s += 1)
    {
        std::cout << "make bv on " << s << std::endl
                  << std::flush;

        bit_vector b1 = bit_vector(pow(2, s), 0);
        int num_1s = 0;
        for (int i = 0; i < pow(2, s); i++)
        {
            // std::cout << i << std::flush;
            if (rand() > RAND_MAX / 2)
            {
                num_1s++;
                (b1)[i] = 1;
            }
            // std::cout << 3 ;
        }
        RankSupport rs1 = RankSupport(&b1);
        SelectSupport ss = SelectSupport(&rs1);
        std::cout << "ranked" << std::endl
                  << std::flush;
        // rank_support_v<> rs1s = rank_support_v(b1);
        for (int i = 0; i < num_ops; i++)
        {
            ranks[i] = 1 + (rand() % num_1s);
        }

        // std::cout << "idx" << std::endl << std::flush;
        // time_t start, end;
        auto start = std::chrono::steady_clock::now();

        /* You can call it like this : start = time(NULL);
         in both the way start contain total time in seconds
         since the Epoch. */
        // time(&start);

        for (int i = 0; i < num_ops; i++)
        {
            // std::cout << i << std::endl << std::flush;
            int r = ranks[i];
            ss.select1(r);
        }

        // Recording end time.
        // time(&end);
        auto end = std::chrono::steady_clock::now();

        std::cout << "rec" << std::endl
                  << std::flush;
        // double elapsed = (double)( end-start);
        std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << std::endl;
        myfile << pow(2, s) << "," << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "\n";
    }
    myfile.close();
}
int main()
{
    get_performance_data_t2();
    get_space_data_t2();
    // test_select();
    // test_rank();
    // get_performance_data_t1();
    // get_space_data_t1();
    // test_rank_save_load();
    // test_select_save_load();
    // t3();
    return 0;
    // std::string name = "sdfs";
    // rs1.save(name);
}