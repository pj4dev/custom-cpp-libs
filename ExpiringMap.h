//
// @file: ExpiringMap.h
// @author: pj4dev.mit@gmail.com
// @url: https://github.com/pj4dev/custom-cpp-libs
//

#include <map>
#include <queue>
#include <chrono>
#include <memory>

namespace pj4dev {

  //
  // Class: ExpiringMap
  // Usage: ExpiringMap<K, V> emap;
  // ----------------------------------------------------------------
  // This template provides an expiring map that its keys with corresponding
  // values can expire after a specific duration (in milliseconds)
  template<typename K, typename V>
  class ExpiringMap {
  private:
      class Item; // forward declaration
      struct ItemCompare {
      	bool operator()(std::weak_ptr<Item> lhs, std::weak_ptr<Item> rhs) {
      		return lhs.lock()->getExpire() > rhs.lock()->getExpire();
      	}
      };

  public:
      ExpiringMap() = default;
      //NOTE: may require the rule of five (especially for deep-copy)

      //
      // Member function: put
      // Usage: emap.put(key, value, duration);
      // ----------------------------------------------------------------
      // This function insertes a key and its associated value into the expiring map
      // with the duration for expiration (in milliseconds). If the given key has
      // already existed in the map, it will be updated and overwritten. No values
      // are returned by this function.
      void put(const K& key, const V& value, long ms);

      //
      // Member function: get
      // Usage: emap.get(key);
      // ----------------------------------------------------------------
      // This function retrieves a value from the given key. If the key does not
      // exist in the expring map or it has already expired, it will return
      // the default value (zero or null).
      V get(const K& key) const;

      //
      // Member function: keys
      // Usage: auto keys = emap.keys();
      // ----------------------------------------------------------------
      // This function returns a vector of keys which are still valid in the expiring
      // map at the partucular point of time.
      std::vector<K> keys() const;

      //
      // Member function: left
      // Usage: auto timeLeft = emap.left(key);
      // ----------------------------------------------------------------
      // This function returns a remaining time (in milliseconds) for the particular key
      // in the expiring map. If the key doesn't exist in the map or has been already expired,
      // it will return zero.
      long left(const K& key) const;

      //
      // Member function: erase
      // Usage: emap.erase(key);
      // ----------------------------------------------------------------
      // This function deletes a key and its associated value from the expiring map.
      // It will have no effects if the key doesn't exist in the map.
      void erase(const K& key) noexcept;

      //
      // Member function: clear
      // Usage: emap.clear();
      // ----------------------------------------------------------------
      // This function removes all elements in the expiring map.
      void clear() noexcept;

      //
      // Member function: size
      // Usage: auto s = emap.size();
      // ----------------------------------------------------------------
      // This function returns the size of non-expired key-value elements in
      // the expiring map at the particular point of time.
      size_t size() const noexcept;

  private:
      class Item {
      public:
          Item() = default;
          Item(const K& k, const V& v, long exp)
            : key_{k}, value_{v}, expire_{exp}{}
          const K& getKey() const noexcept { return key_; }
  	      const V& getValue() const noexcept { return value_; }
          long getExpire() const noexcept { return expire_; }
      private:
  	      K key_;
  	      V value_;
  	      long expire_;
      };

      mutable std::priority_queue<std::weak_ptr<Item>, std::vector<std::weak_ptr<Item>>, ItemCompare> expired_queue_;
      mutable std::map<K, std::shared_ptr<Item>> internal_map_;

      static long current_time() noexcept {
  	     return std::chrono::duration_cast<std::chrono::milliseconds>(
  		       std::chrono::system_clock::now().time_since_epoch()
  	     ).count();
      }
      void clearExpired() const;
  };

  template<typename K, typename V>
  inline void ExpiringMap<K, V>::put(const K& key, const V& value, long ms) {
      auto expired_time = current_time() + ms;
      auto item = std::make_shared<Item>(key, value, expired_time);
      auto res = internal_map_.find(key);
      if (res != internal_map_.end()) {
      	internal_map_.erase(res);
      }
      internal_map_.emplace(key, item);
      expired_queue_.push(item);
      clearExpired();
  }

  template<typename K, typename V>
  inline V ExpiringMap<K, V>::get(const K& key) const {
      auto value = V{};
      auto res = internal_map_.find(key);
      if (res != internal_map_.end()){
      	if (res->second->getExpire() > current_time())
  		    value = res->second->getValue();
      }
      //clearExpired();
      return value;
  }

  template<typename K, typename V>
  inline std::vector<K> ExpiringMap<K, V>::keys() const {
      auto curtime = current_time();
      auto keys = std::vector<K>{};
      std::for_each(internal_map_.cbegin(), internal_map_.cend(), [&keys, &curtime](const auto& a) {
        if (a.second->getExpire() > curtime) keys.push_back(a.first);
      });
      std::sort(keys.begin(), keys.end(), [this](const auto& a, const auto& b) {
        return (internal_map_[a]->getExpire() != internal_map_[b]->getExpire())?
          internal_map_[a]->getExpire() < internal_map_[b]->getExpire() : a < b;
      });
      return keys;
  }

  template<typename K, typename V>
  inline long ExpiringMap<K, V>::left(const K& key) const {
      auto expired_time = 0U;
      auto res = internal_map_.find(key);
      if (res != internal_map_.end()){
      	expired_time = res->second->getExpire() - current_time();
      }
      //clearExpired();
      return expired_time;
  }

  template<typename K, typename V>
  inline void ExpiringMap<K, V>::erase(const K& key) noexcept {
      internal_map_.erase(key);
  }

  template<typename K, typename V>
  inline void ExpiringMap<K, V>::clear() noexcept {
      while(!expired_queue_.empty()) {
  	    expired_queue_.pop();
      }
      internal_map_.clear();
  }

  template<typename K, typename V>
  inline size_t ExpiringMap<K, V>::size() const noexcept {
      clearExpired();
      return internal_map_.size();
  }

  template<typename K, typename V>
  inline void ExpiringMap<K, V>::clearExpired() const {
      while(!expired_queue_.empty()) {
  	     if (auto top = expired_queue_.top().lock()) {
  		       if (top->getExpire() > current_time()) break;
  	 	       internal_map_.erase(top->getKey());
  	     }
  	     expired_queue_.pop();
      }
  }

}
