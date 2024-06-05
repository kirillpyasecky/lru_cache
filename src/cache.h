// --------------------------------------------
// chache.h
// --------------------------------------------
// Contains the LRUCache class, which is a 
// simple C++ LRU cache implementation.
// --------------------------------------------

#pragma once

#include <iterator>
#include <list>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cache {
    template <class Key, class Value>
    class LRUCache {
       public:
        using key_type = Key;
        using value_type = Value;
        using list_type = std::list<std::pair<key_type, value_type>>;
        using list_iterator = typename list_type::iterator;
        using map_type = std::unordered_map<key_type, list_iterator>;

        explicit LRUCache(size_t capasity) : capasity_(capasity) {
        }

        void Clear() {
            cache_.clear();
            hash_.clear();
        }

        void Resize(size_t size) {
            if (capasity_ <= size) {
                capasity_ = size;
                return;
            }

            capasity_ = size;
            while (Size() > capasity_) {
                hash_.erase(cache_.back().first);
                cache_.pop_back();
            }

        }

        [[nodiscard]] bool Contains(key_type key) const {
            return hash_.contains(key);
        }

        [[nodiscard]] bool Empty() const {
            return cache_.empty();
        }

        [[nodiscard]] size_t Size() const {
            return cache_.size();
        }

        [[nodiscard]] size_t Capasity() const {
            return capasity_;
        }

        [[nodiscard]] bool Full() const {
            return (cache_.size() == capasity_); 
        }

        [[maybe_unused]] bool Insert(key_type key, value_type value) {
            auto hit = hash_.find(key);

            // lookup value in the cache
            if (hit == hash_.end()) {
                if (Full()) {
                    // if full, erase last used value from list (cache_)
                    hash_.erase(cache_.back().first);
                    cache_.pop_back();
                }
                // add to first position in list (cache_)
                cache_.emplace_front(key, value);
                hash_.emplace(key, cache_.begin());
                return false;
            }

            // if found, move to begin of list (cache_)
            auto eltit = hit->second;
            if (eltit != cache_.begin()) {
                cache_.splice(cache_.begin(), cache_, eltit, std::next(eltit));
            }
            return true;
        }

        [[nodiscard]] std::optional<value_type> Get(const key_type &key) {
            // lookup value in the hash_
            typename map_type::iterator hit = hash_.find(key);
            if(hit == hash_.end()){
                // value not in hash
                return std::optional<value_type>(); // aka std::nullopt_t
            }

            // return the value, but first update its place in the most
            // recently used list
            typename list_type::iterator value_it = hit->second;
            if(value_it == cache_.begin()){
                 // the item is already at the front of the most recently
                // used list so just return it
                return std::make_optional<value_type>(hit->second->second);
            }
                
            // move item to the front of the most recently used list
            cache_.erase(value_it);
            const value_type &value = hit->second->second;
            cache_.push_front(std::make_pair(key, value));

            // update iterator in map
            hash_[key] = cache_.begin();

            // return the value
            return std::make_optional<value_type>(value);
        }

       private:
        size_t capasity_{0};
        std::list<std::pair<key_type, value_type>> cache_{};
        std::unordered_map<key_type, list_iterator> hash_{};
    };

} // namespace cache
