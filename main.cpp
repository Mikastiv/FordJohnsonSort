#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

constexpr size_t MAX_ARRAY_SIZE = 4'096; // max number of elements in the array to sort
constexpr size_t N_ELEM_IN_PAIR = 2;

std::vector<int>
generate_random_numbers(const size_t count, const int lowest, const int highest) {
    std::random_device rd{};
    std::mt19937 rng{ rd() };

    std::uniform_int_distribution<int> distribution{ lowest, highest };

    std::vector<int> numbers{};
    numbers.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        numbers.push_back(distribution(rng));
    }
    return numbers;
}

template <typename It>
void
print_array(It start, It end) {
    while (start != end) {
        std::cout << *start << ' ';
        ++start;
    }
    std::cout << '\n';
}

void
sort_inside_pairs(std::vector<int>& array) {
    if (array.size() < 2) return;

    for (size_t i = 0; i < array.size() - 1; i += 2) {
        if (array[i] > array[i + 1]) std::swap(array[i], array[i + 1]);
    }
}

void
merge_pair_arrays(std::vector<int>& array, const size_t left, const size_t middle, const size_t right) {
    // adjust indices because we're counting in pairs
    const size_t l = left * N_ELEM_IN_PAIR;
    const size_t m = middle * N_ELEM_IN_PAIR;
    const size_t r = right * N_ELEM_IN_PAIR;

    const size_t left_length = m - l + N_ELEM_IN_PAIR;
    const size_t right_length = r - m;

    // safety checks
    assert(left_length <= MAX_ARRAY_SIZE / 2);
    assert(right_length <= MAX_ARRAY_SIZE / 2);

    std::array<int, MAX_ARRAY_SIZE / 2> tmp_left;  // could do `new int[left_length]` instead; using stack for speed
    std::array<int, MAX_ARRAY_SIZE / 2> tmp_right; // same as above

    // copy both sides to temp arrays
    std::copy(&array[l], &array[l + left_length], tmp_left.begin());
    std::copy(&array[m + N_ELEM_IN_PAIR], &array[m + N_ELEM_IN_PAIR + right_length], tmp_right.begin());

    size_t i = 0;
    size_t j = 0;
    for (size_t k = l; k < r + 1; k += N_ELEM_IN_PAIR) {
        if (i < left_length && (j >= right_length || tmp_left[i + 1] < tmp_right[j + 1])) {
            // copy pair from left
            array[k] = tmp_left[i];
            array[k + 1] = tmp_left[i + 1];
            i += N_ELEM_IN_PAIR;
        } else {
            // copy pair from right
            array[k] = tmp_right[j];
            array[k + 1] = tmp_right[j + 1];
            j += N_ELEM_IN_PAIR;
        }
    }
}

void
merge_sort_pairs_recursion(std::vector<int>& array, const size_t left, const size_t right) {
    if (left >= right) return; // recursion end condition

    const size_t middle = left + (right - left) / 2;
    merge_sort_pairs_recursion(array, left, middle);      // sort left part
    merge_sort_pairs_recursion(array, middle + 1, right); // sort right part

    merge_pair_arrays(array, left, middle, right); // merge two sorted parts
}

void
merge_sort_pairs(std::vector<int>& array) {
    const bool is_odd = array.size() % 2 != 0;
    merge_sort_pairs_recursion(array, 0, (array.size() - 1) / N_ELEM_IN_PAIR - (is_odd ? 1 : 0));
}

std::vector<int>
create_main_chain(const std::vector<int>& array) {
    std::vector<int> out;
    out.reserve(array.size() / N_ELEM_IN_PAIR);
    // main chain is created using the larger number from each pairs
    // those numbers are the second elements in the pairs
    // e.g. in pair[i, i + 1] the largest is pair[i + 1]
    for (size_t i = 1; i < array.size(); i += N_ELEM_IN_PAIR) {
        out.push_back(array[i]);
    }

    return out;
}

std::vector<int>
create_pend_elements(const std::vector<int>& array) {
    std::vector<int> out;
    out.reserve(array.size() / N_ELEM_IN_PAIR);
    // pend elements are the smaller number from each pairs
    // those numbers are the first elements in the pairs
    // e.g. in pair[i, i + 1] the smallest is pair[i]
    for (size_t i = 0; i < array.size(); i += N_ELEM_IN_PAIR) {
        out.push_back(array[i]);
    }

    return out;
}

// get nth jacobsthal number
int
jacobsthal(const int n) {
    return (std::pow(2, n + 1) + std::pow(-1, n)) / 3;
}

void
binary_insert(std::vector<int>& array, const size_t start, const size_t end, const int number) {
    if (start == end) {
        if (array[start] > number)
            array.insert(array.begin() + start, number);
        else
            array.insert(array.begin() + start + 1, number);
        return;
    }

    const size_t middle = (start + end) / 2;
    if (array[middle] > number)
        return binary_insert(array, start, middle, number);
    else
        return binary_insert(array, middle + 1, end, number);
}

void
merge_insertion_sort(std::vector<int>& array) {
    // sort pairs, leaving odd number if present
    // e.g.
    // 19 elements:
    //               24 10 10 97 42 32 40 84 29 26 66 96 57 13 46 30 91 48 11 <- odd
    //               ----- ----- ----- ----- ----- ----- ----- ----- -----
    // becomes ->    10 23 10 97 32 42 40 84 26 29 66 96 13 57 30 46 48 91 11
    sort_inside_pairs(array);

    // regular merge sort over the pairs, while only using the bigger element of the pairs for comparisons
    //
    //               10 23 10 97 32 42 40 84 26 29 66 96 13 57 30 46 48 91 11
    //               ----- ----- ----- ----- ----- ----- ----- ----- -----
    // becomes ->    10 23 26 29 32 42 30 46 13 57 40 84 48 91 66 96 10 97 11
    merge_sort_pairs(array);

    // main chain is the larger element from every pairs
    //
    //               10 23 26 29 32 42 30 46 13 57 40 84 48 91 66 96 10 97 11
    //               ----- ----- ----- ----- ----- ----- ----- ----- -----
    // main chain -> 23 29 42 46 57 84 91 96 97
    std::vector<int> main_chain = create_main_chain(array);

    // pend is the smallest element from every pairs + odd element if present
    //
    //               10 23 26 29 32 42 30 46 13 57 40 84 48 91 66 96 10 97 11
    //               ----- ----- ----- ----- ----- ----- ----- ----- -----
    // pend       -> 10 26 32 30 13 40 48 66 10 11
    std::vector<int> pend = create_pend_elements(array);

    // insert pend elements into main chain following "jacobsthal" order
    // jacobsthal numbers: 1, 1, 3, 5, 11, 43, 683, ...
    //
    //                 (n1 will always be inserted first)
    // given:          n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14
    // the order is:   n3, n2,   n5, n4,   n11, n10, n9, n8, n7, n6,   n14, n13, n12
    //
    // the pattern follows jacobsthal(k) * 2, backwards in the array
    // jacobsthal numbers * 2: 2, 2, 6, 10, 22, 86, ...
    //
    // note that I am removing elements from pend in the drawings, but I am not doing
    // so in the actual code implementation to keep the pend indices from changing
    //
    // e.g.
    // main chain -> 23-29-42-46-57-84-91-96-97
    //               |  |  |  |  |  |  |  |  |
    // pend       -> 10 26 32 30 13 40 48 66 10 11
    // pend index:   0  1  2  3  4  5  6  7  8  9
    //
    //
    // always insert the first pend number
    // main chain -> 10-23-29-42-46-57-84-91-96-97
    //                     |  |  |  |  |  |  |  |
    // pend       ->       26 32 30 13 40 48 66 10 11
    // pend index:      0  1  2  3  4  5  6  7  8  9
    //
    // then, we need to follow the order described previously:
    // the first jacobsthal number * 2 = 2
    // lets say we have an index i and k as the current jacobsthal number starting at 1:
    // int i = 0; int k = 1;
    //
    // main chain -> 10-23-29-42-46-57-84-91-96-97
    //                     |  |  |  |  |  |  |  |
    // pend       ->      26 32 30 13 40 48 66 10 11
    //                  ^
    //                  i (the index 0 is valid even is the number is now in the main chain)
    // pend index:      0  1  2  3  4  5  6  7  8  9
    //
    // we now have to go forward by the kth jacobsthal number * 2, in this case: jacobsthal(1) * 2 = 2
    // lets also save i before increasing it to know where to stop; I will call it s
    // int s = i;
    // main chain -> 10-23-29-42-46-57-84-91-96-97
    //                     |  |  |  |  |  |  |  |
    // pend       ->       26 32 30 13 40 48 66 10 11
    //                  ^     ^
    //                  s     i
    // pend index:      0  1  2  3  4  5  6  7  8  9
    //
    // we then iterate backwards until we reach the previous i (aka s), while inserting pend[i] inside the main chain
    // the binary insertion has to be done with the low bound as 0 (the start of main chain) and the index - 1 of
    // the value paired with the pend value we are inserting
    //
    // the slice is represented with [] here
    // main chain -> [10-23-29]-42-46-57-84-91-96-97
    //                      |   |  |  |  |  |  |  |
    // pend       ->        26  32 30 13 40 48 66 10 11
    //                   ^      ^
    //                   s      i
    // pend index:       0  1   2  3  4  5  6  7  8  9
    //
    // so we insert 32 in the slice [10,23,29] with a binary insertion
    //
    // main chain -> 10-23-29-32-42-46-57-84-91-96-97
    //                       \      |  |  |  |  |  |
    // pend       ->          26    30 13 40 48 66 10 11
    //                     ^     ^
    //                     s     i
    // pend index:         0  1  2  3  4  5  6  7  8  9
    //
    // next we decrement i because we have not yet reached s
    //
    // main chain -> 10-23-29-32-42-46-57-84-91-96-97
    //                       \      |  |  |  |  |  |
    // pend       ->          26    30 13 40 48 66 10 11
    //                     ^  ^
    //                     s  i
    // pend index:         0  1  2  3  4  5  6  7  8  9
    //
    // insert 26 with the same logic
    //
    // main chain -> [10-23]-29-32-42-46-57-84-91-96-97
    //                        \       |  |  |  |  |  |
    // pend       ->           26     30 13 40 48 66 10 11
    //                     ^   ^
    //                     s   i
    // pend index:         0   1  2   3  4  5  6  7  8  9
    //
    // main chain -> 10-23-26-29-32-42-46-57-84-91-96-97
    //                                 |  |  |  |  |  |
    // pend       ->                   30 13 40 48 66 10 11
    //                     ^   ^
    //                     s   i
    // pend index:         0   1   2   3  4  5  6  7  8  9
    //
    // next when we decrement i again, we reach s; we now need a new jacosthal number (2nd): jacobsthal(2) * 2 = 2
    // before we move i, it has to be reset to where it was at the beginning of the iteration (i = 2 in this case)
    //
    // main chain -> 10-23-26-29-32-42-46-57-84-91-96-97
    //                                 |  |  |  |  |  |
    // pend       ->                   30 13 40 48 66 10 11
    //                     ^       ^
    //                     s       i
    // pend index:         0   1   2   3  4  5  6  7  8  9
    //
    // now move i forward again by jacobsthal(2) * 2, which is 2 again
    // and we also have to move s to where is i before moving it
    //
    // main chain -> 10-23-26-29-32-42-46-57-84-91-96-97
    //                                 |  |  |  |  |  |
    // pend       ->                   30 13 40 48 66 10 11
    //                             ^      ^
    //                             s      i
    // pend index:         0   1   2   3  4  5  6  7  8  9
    //
    // we now apply the same logic as before, note the binary insertion slice in each insertion
    //
    // main chain -> [10-23-26-29-32-42]-46-57-84-91-96-97
    //                                   |  |  |  |  |  |
    // pend       ->                     30 13 40 48 66 10 11
    //                               ^      ^
    //                               s      i
    // pend index:         0    1    2   3  4  5  6  7  8  9
    //
    // main chain -> 10-13-23-26-29-32-42-46-57-84-91-96-97
    //                                    |     |  |  |  |
    // pend       ->                      30    40 48 66 10 11
    //                               ^       ^
    //                               s       i
    // pend index:         0    1    2    3  4  5  6  7  8  9
    //
    // main chain -> [10-13-23-26-29-32-42]-46-57-84-91-96-97
    //                                      |     |  |  |  |
    // pend       ->                        30    40 48 66 10 11
    //                                 ^    ^
    //                                 s    i
    // pend index:         0     1     2    3  4  5  6  7  8  9
    //
    // main chain -> 10-13-23-26-29-30-32-42-46-57-84-91-96-97
    //                                             |  |  |  |
    // pend       ->                               40 48 66 10 11
    //                                 ^     ^
    //                                 s     i
    // pend index:         0     1     2     3  4  5  6  7  8  9
    //
    // reset i and move s up
    //
    // main chain -> 10-13-23-26-29-30-32-42-46-57-84-91-96-97
    //                                             |  |  |  |
    // pend       ->                               40 48 66 10 11
    //                                          ^
    //                                         si
    // pend index:         0     1     2     3  4  5  6  7  8  9
    //
    // the next jacobsthal number is now: jacobsthal(3) * 2 = 6
    // since we don't have 6 elements left to insert, we can move i forward by the number of elements left in pend
    // there are 5 numbers left here
    //
    // main chain -> 10-13-23-26-29-30-32-42-46-57-84-91-96-97
    //                                             |  |  |  |
    // pend       ->                               40 48 66 10 11
    //                                          ^              ^
    //                                          s              i
    // pend index:         0     1     2     3  4  5  6  7  8  9
    //
    // keep binary inserting like before until we inserted every numbers from pend
    //
    // main chain -> [10-13-23-26-29-30-32-42-46-57]-84-91-96-97
    //                                               |  |  |  |
    // pend       ->                                 40 48 66 10 11
    //                                           ^               ^
    //                                           s               i
    // pend index:         0     1     2     3   4   5  6  7  8  9
    //
    // main chain -> 10-11-13-23-26-29-30-32-42-46-57-84-91-96-97
    //                                                |  |  |  |
    // pend       ->                                  40 48 66 10
    //                                           ^                ^
    //                                           s                i
    // pend index:         0     1     2     3   4    5  6  7  8  9
    //
    // ...
    //
    // main chain -> 10-10-11-13-23-26-29-30-32-40-42-46-48-57-66-84-91-96-97
    // pend       ->
    //
    // and now we're done!

    main_chain.insert(main_chain.begin(), pend.front());

    size_t insertion_counter = 1; // how many pend elements have been inserted (for right offset during binary inserts)
    size_t i = 0;                 // index in pend
    size_t jacobsthal_idx = 1;    // current jacobsthal number
    while (true) {
        const int distance_forward = 2 * jacobsthal(jacobsthal_idx);
        if (i + distance_forward >= pend.size()) break; // break if move forward is out of bounds

        const size_t start = i;
        i += distance_forward;
        while (i > start) {
            binary_insert(main_chain, 0, i + insertion_counter - 1, pend[i]);
            ++insertion_counter;
            --i;
        }
        i += distance_forward;
        ++jacobsthal_idx;
    }

    // insert rest of pend if any
    if (i < pend.size()) {
        const size_t start = i;
        i = pend.size() - 1;

        while (i > start) {
            binary_insert(main_chain, 0, main_chain.size() - 1, pend[i]);
            --i;
        }
    }

    array = std::move(main_chain);
}

int
main() {
    std::vector<int> numbers = generate_random_numbers(42, 1, 99);
    std::vector<int> array_to_sort = numbers;

    std::cout << "before: ";
    print_array(array_to_sort.begin(), array_to_sort.end());

    merge_insertion_sort(array_to_sort);

    std::cout << "after:  ";
    print_array(array_to_sort.begin(), array_to_sort.end());

    std::cout << "sorted?: " << std::boolalpha << std::is_sorted(array_to_sort.begin(), array_to_sort.end());
}
