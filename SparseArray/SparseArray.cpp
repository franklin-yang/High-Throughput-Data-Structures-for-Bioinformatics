#include <sdsl/suffix_arrays.hpp>
#include <fstream>
/* printf */
#include <math.h>
#include <vector>
#include "../RankSupport/RankSupport.cpp"
using namespace sdsl;


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

// code below comes from geeksforgeeks
// https://www.geeksforgeeks.org/shuffle-a-given-array-using-fisher-yates-shuffle-algorithm/
// A utility function to swap to integers
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

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

int main()
{
    // get_performance_data_t2();
    // get_space_data_t2();
    // test_select();
    // test_rank();
    // get_performance_data_t1();
    // get_space_data_t1();
    // test_rank_save_load();
    // test_select_save_load();
    t3();
    return 0;
    // std::string name = "sdfs";
    // rs1.save(name);
}