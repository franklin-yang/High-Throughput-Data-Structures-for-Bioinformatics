#include <sdsl/suffix_arrays.hpp>
#include <fstream>
/* printf */
#include <math.h>
#include <vector>
using namespace sdsl;

void print_int_vec(int_vector<> iv)
{
    for (int i = 0; i < iv.size(); i++)
    {
        std::cout << iv[i] << " ";
    }
    std::cout << std::endl;
}

class RankSupport
{
public:
    bit_vector *bv;
    int_vector<0> superblocks;
    int_vector<0> blocks;
    uint64_t total_bits;
    RankSupport(bit_vector *bit_vec)
    {

        std::cout << "const start  " << std::endl
                  << std::flush;
        bv = bit_vec;
        size_t num_bits = bit_vec->size();
        size_t num_bits_per_block = ceil(log2(num_bits));
        const size_t num_bits_per_superblock = pow(num_bits_per_block, 2);
        size_t block_element_size_bits = ceil(log2(num_bits_per_superblock + 1));
        size_t superblock_element_size_bits = ceil(log2(num_bits + 1));
        const size_t num_superblocks = ceil((float)num_bits / num_bits_per_superblock);
        size_t num_blocks_per_superblock = (num_bits_per_superblock / num_bits_per_block);
        size_t num_blocks = num_superblocks * num_blocks_per_superblock;
        // size_t num_blocks = ceil(log2(num_bits));

        std::cout << "aloc sb  " << std::endl
                  << std::flush;
        superblocks = int_vector<>(num_superblocks, 0, superblock_element_size_bits);

        std::cout << "aloc b  " << num_blocks << " " << block_element_size_bits << std::endl
                  << std::flush;
        blocks = int_vector<>(num_blocks, 0, block_element_size_bits);

        total_bits = block_element_size_bits * num_blocks + superblock_element_size_bits * num_superblocks;
        // populate blocks
        std::cout << "fill blocks+sb  " << std::endl
                  << std::flush;
        for (size_t bit_it = 0; bit_it < num_bits; bit_it++)
        {
            if ((*bv)[bit_it] == 1)
            {
                // bit and index bit_it is 1
                superblocks[(bit_it / num_bits_per_superblock) + 1] = superblocks[bit_it / num_bits_per_superblock + 1] + 1;
                blocks[bit_it / num_bits_per_block + 1] = blocks[bit_it / num_bits_per_block + 1] + 1;
            }
        }
        // std::cout << std::endl;
        std::cout << "prefix sum  " << block_element_size_bits << std::endl;
        // print_int_vec(blocks);
        // prefix sum
        for (size_t sb_it = 0; sb_it < num_superblocks; sb_it++)
        {
            blocks[sb_it * num_blocks_per_superblock] = 0;
            for (size_t block_it = 1; block_it < num_blocks_per_superblock; block_it++)
            {
                int idx = (sb_it * num_blocks_per_superblock) + block_it;
                int temp = blocks[idx] + blocks[idx - 1];
                blocks[idx] = temp;
                // print_int_vec(blocks);
                // std::cout << (sb_it * num_blocks_per_superblock) + block_it << std::endl;
                // std::cout << blocks[(sb_it * num_blocks_per_superblock) + block_it] << std::endl;
                // std::cout << blocks[(sb_it * num_blocks_per_superblock) + block_it-1]<< std::endl;
            }
            // std::cout << "dsf";
            if (sb_it > 0)
            {
                superblocks[sb_it] = superblocks[sb_it] + superblocks[sb_it - 1];
            }
        }

        std::cout << "const done  " << std::endl;
        // print_bit_vec(bit_vec);
        // std::cout << "num bits " << num_bits << " " << ceil(log2(num_bits)) << std::endl;
        // std::cout << "num blpsb" << num_blocks_per_superblock << std::endl;
        // std::cout << "num sb " << num_superblocks << std::endl;
        // std::cout << "num besb  " << block_element_size_bits << std::endl;
        // std::cout << "num bitpb  " << num_bits_per_block << std::endl;
        // print_int_vec(*bv);
        // std::cout << 3 << std::endl;
        // print_int_vec(superblocks);
        // print_int_vec(blocks);
        // print_structures();
    }
    void print_bit_blocks()
    {
        size_t num_bits = bv->size();
        size_t num_bits_per_block = ceil(log2(num_bits));
        for (int i = 0; i < num_bits; i++)
        {
            if (i % num_bits_per_block == 0)
            {
                std::cout << " |";
            }
            std::cout << (*bv)[i] << " ";
        }
        std::cout << std::endl;
    }

    void print_superblocks()
    {
        size_t num_bits = bv->size();
        size_t num_bits_per_block = ceil(log2(num_bits));
        const size_t num_bits_per_superblock = pow(num_bits_per_block, 2);
        for (int i = 0; i < bv->size(); i++)
        {
            if (i % num_bits_per_superblock == 0)
            {
                std::cout << superblocks[i / num_bits_per_superblock] << "|";
            }
            else
            {
                std::cout << "  ";
            }
        }
        std::cout << std::endl;
    }
    void print_blocks()
    {
        size_t num_bits = bv->size();
        size_t num_bits_per_block = ceil(log2(num_bits));
        for (int i = 0; i < blocks.size(); i++)
        {
            for (int j = 0; j < num_bits_per_block; j++)
            {
                if (j == num_bits_per_block / 2)
                {

                    std::cout << (blocks)[i] << " ";
                }
                else
                {
                    std::cout << "  ";
                }
            }
            std::cout << " | ";
        }
        std::cout << std::endl;
    }
    void print_structures()
    {
        int num_bits = bv->size();
        size_t num_bits_per_block = ceil(log2(num_bits));
        const size_t num_bits_per_superblock = pow(num_bits_per_block, 2);
        size_t block_element_size_bits = ceil(log2(num_bits_per_superblock + 1));
        size_t superblock_element_size_bits = ceil(log2(num_bits_per_superblock + 1));
        const size_t num_superblocks = ceil((float)num_bits / num_bits_per_superblock);
        size_t num_blocks_per_superblock = (num_bits_per_superblock / num_bits_per_block);
        size_t num_blocks = ceil((float)num_bits / num_bits_per_block);
        // print_bit_blocks();
        // print_blocks();
        // print_superblocks();
    }

    uint64_t rank1(uint64_t i)
    {
        // if (i == 0) {
        //     return 0;
        // }
        // i--;
        size_t num_bits = bv->size();
        // size_t num_bits = bit_vec->size();
        size_t num_bits_per_block = ceil(log2(num_bits));
        size_t block_element_size_bits = ceil(log2(num_bits_per_block + 1));
        const size_t num_bits_per_superblock = pow(num_bits_per_block, 2);
        const size_t num_superblocks = ceil(num_bits / num_bits_per_superblock);
        size_t num_blocks_per_superblock = (num_bits_per_superblock / num_bits_per_block);
        size_t num_blocks = num_superblocks * num_blocks_per_superblock;
        uint64_t sb_idx = i / num_bits_per_superblock;
        uint64_t b_idx = i / num_bits_per_block;
        uint64_t r_s = superblocks[i / num_bits_per_superblock];
        uint64_t r_b = blocks[i / num_bits_per_block];
        uint64_t block_bit_idx_l = (i / num_bits_per_block) * num_bits_per_block;
        // std::cout << "bbi " << block_bit_idx_l << std::endl;

        // uint64_t block_bit_idx_r = block_bit_idx_l + block_element_size_bits;
        // uint64_t chunk_idx_left = block_bit_idx_l / 64;
        // uint64_t chunk_idx_right = block_bit_idx_r / 64;
        uint64_t block_bits = bv->get_int(block_bit_idx_l, i - block_bit_idx_l + 1);
        // uint64_t shift = num_bits_per_block - (i - block_bit_idx_l) - 1;
        // uint64_t shifted_b = block_bits >> shift;
        uint64_t r_p = std::popcount(block_bits);
        // std::cout << bv->get_int(0,3) << std::endl;
        // std::cout << "i:" << i << " r_s:" << r_s << " r_b:" << r_b << " r_p:" << r_p << " " << block_bit_idx_l << " bb:" << block_bits << " "  << num_bits_per_block << std::endl;
        // std::cout << bv->get_int(4,4) << "kmj  " <<   bv->get_int(8,4) << " " <<  bv->get_int(12,4) << " " << bv->get_int(0,4)  << " " << bv->get_int(0,0) << " " << block_bits << " " << shift << num_bits_per_block << std::endl;
        return r_s + r_b + r_p;
        // cout << "rank_b(3) = " << std::popcount( shifted_b ) << "\n";;
    }

    uint64_t overhead()
    {
        return total_bits;
    }

    void save(std::string &fname)
    {
        std::ofstream seqOut(fname);
        bv->serialize(seqOut);
        superblocks.serialize(seqOut);
        blocks.serialize(seqOut);
        seqOut.close();
    }

    void load(std::string &fname)
    {
        std::ifstream seqOut(fname);
        bv->load(seqOut);
        superblocks.load(seqOut);
        blocks.load(seqOut);
        seqOut.close();
    }
};

void print_bit_vec(bit_vector *bv)
{
    std::cout << "print bv" << std::endl;
    RankSupport rs = RankSupport(bv);
    for (int i = 0; i < bv->size(); i++)
    {
        std::cout << (*bv)[i] << "b" << rs.rank1(i) << " ";
    }
    std::cout << std::endl;
}


void get_performance_data_t1()
{
    std::ofstream myfile;
    myfile.open("t1.csv");
    const int num_ops = 1000000;
    // int n = 1000;
    int idxs[num_ops];
    myfile << "N,time for " << num_ops << " operations\n";
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
        // rank_support_v<> rs1s = rank_support_v(b1);
        for (int i = 0; i < num_ops; i++)
        {
            idxs[i] = rand() % (int)pow(2, s);
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
            int idx = idxs[i];
            rs1.rank1(idx);
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

void get_space_data_t1()
{
    std::ofstream myfile;
    myfile.open("t1space.csv");
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
        // rank_support_v<> rs1s = rank_support_v(b1);
        // double elapsed = (double)( end-start);
        // std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()<< std::endl;
        myfile << pow(2, s) << "," << rs1.overhead() << "\n";
    }
    myfile.close();
}

void random_populate(bit_vector &bv)
{
    // std::cout << (bv == NULL) << " 3" << std::endl << std::flush;
    for (bit_vector::size_type i = 0; i < bv.size(); i++)
    {
        // std::cout << i << std::flush;
        if (rand() > RAND_MAX / 2)
            (bv)[i] = 1;
        // std::cout << 3 ;
    }
}

bool test_rank()
{
    for (int s = 1; s < 25; s += 1)
    {
        std::cout << "make bv on " << s << std::endl
                  << std::flush;

        int size = pow(2, s);
        bit_vector b1 = bit_vector(size, 0);
        random_populate(b1);
        RankSupport rs1 = RankSupport(&b1);
        rank_support_v<> rs1s = rank_support_v(&b1);
        // print_bit_vec(&b1);
        // int rs[num1s];
        for (int i = 0; i < size; i++)
        {
            if (rs1.rank1(i) != rs1s.rank(i + 1))
            {
                std::cout << "r" << i << " got:" << rs1.rank1(i) << " ex:" << rs1s.rank(i + 1) << std::endl;
                return false;
            }
            else
            {
                // std::cout << "r" << i << " is:" << ss1.select1(i) << " ex:" <<ss1s.select(i) << std::endl;
            }
        }
        std::cout << s << " complete " << std::endl
                  << std::flush;
    }

    return true;
}

bool test_rank_save_load()
{
    for (int s = 1; s < 25; s += 1)
    {
        std::cout << "make bv on " << s << std::endl
                  << std::flush;

        int size = pow(2, s);
        bit_vector b1 = bit_vector(size, 0);
        random_populate(b1);
        RankSupport rs1 = RankSupport(&b1);
        std::string fname = "temp";
        rs1.save(fname);
        
        bit_vector b2 = bit_vector(size, 0);
        RankSupport rs2 = RankSupport(&b2);
        rs2.load(fname);
        
        for (int i = 0; i < size; i++)
        {
            int ex = rs1.rank1(i);
            int got = rs2.rank1(i);
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

// int main()
// {
//     // get_performance_data_t2();
//     // get_space_data_t2();
//     // test_select();
//     test_rank();
//     // get_performance_data_t1();
//     // get_space_data_t1();
//     test_rank_save_load();
//     // test_select_save_load();
//     // t3();
//     return 0;
//     // std::string name = "sdfs";
//     // rs1.save(name);
// }