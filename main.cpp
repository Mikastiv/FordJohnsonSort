#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

constexpr size_t MAX_ARRAY_SIZE = 4'096; // max number of elements in the array to sort

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

std::vector<std::pair<int, int>>
create_pair_array(const std::vector<int>& array) {
    std::vector<std::pair<int, int>> out;
    for (size_t i = 0; i < array.size() - 1; i += 2) {
        out.push_back({ array[i], array[i + 1] });
    }
    return out;
}

void
merge_pair_arrays(std::vector<int>& array, const size_t left, const size_t middle, const size_t right) {
    const size_t left_length = middle - left + 1;
    const size_t right_length = right - middle;

    // safety checks
    assert(left_length <= MAX_ARRAY_SIZE / 2);
    assert(right_length <= MAX_ARRAY_SIZE / 2);

    std::array<int, MAX_ARRAY_SIZE / 2> tmp_left;  // could do `new int[left_length]` instead; using stack for speed
    std::array<int, MAX_ARRAY_SIZE / 2> tmp_right; // same as above

    // copy both sides to temp arrays
    std::copy(&array[left * 2], &array[(left + left_length) * 2], tmp_left.begin());
    std::copy(&array[(middle + 1) * 2], &array[(middle + 1 + right_length) * 2], tmp_right.begin());

    size_t i = 0;
    size_t j = 0;
    for (size_t k = left; k < right + 1; ++k) {
        if (i < left_length && (j >= right_length || tmp_left[i * 2 + 1] < tmp_right[j * 2 + 1])) {
            array[k * 2] = tmp_left[i * 2];
            array[k * 2 + 1] = tmp_left[i * 2 + 1];
            ++i;
        } else {
            array[k * 2] = tmp_right[j * 2];
            array[k * 2 + 1] = tmp_right[j * 2 + 1];
            ++j;
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
    merge_sort_pairs_recursion(array, 0, (array.size() - 1) / 2 - (is_odd ? 1 : 0));
}

int
main() {
    std::vector<int> numbers = generate_random_numbers(21, 1, 200);

    print_array(numbers.begin(), numbers.end());

    const bool odd_number_of_elements = numbers.size() % 2 != 0;

    sort_inside_pairs(numbers);

    print_array(numbers.begin(), numbers.end());

    std::vector<std::pair<int, int>> pairs = create_pair_array(numbers);
    merge_sort_pairs(numbers);

    print_array(numbers.begin(), numbers.end());
}
