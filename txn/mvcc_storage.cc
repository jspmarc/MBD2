// Author: Kun Ren (kun.ren@yale.edu)
// Modified by Daniel Abadi

#include "txn/mvcc_storage.h"

// Init the storage
void MVCCStorage::InitStorage() {
  for (int i = 0; i < 1000000;i++) {
    Write(i, 0, 0);
    Mutex* key_mutex = new Mutex();
    mutexs_[i] = key_mutex;
  }
}

// Free memory.
MVCCStorage::~MVCCStorage() {
  for (unordered_map<Key, deque<Version*>*>::iterator it = mvcc_data_.begin();
       it != mvcc_data_.end(); ++it) {
    delete it->second;          
  }
  
  mvcc_data_.clear();
  
  for (unordered_map<Key, Mutex*>::iterator it = mutexs_.begin();
       it != mutexs_.end(); ++it) {
    delete it->second;          
  }
  
  mutexs_.clear();
}

// Lock the key to protect its version_list. Remember to lock the key when you read/update the version_list 
void MVCCStorage::Lock(Key key) {
  mutexs_[key]->Lock();
}

// Unlock the key.
void MVCCStorage::Unlock(Key key) {
  mutexs_[key]->Unlock();
}

// MVCC Read
bool MVCCStorage::Read(Key key, Value* result, int txn_unique_id) {
  // CPSC 438/538:
  //
  // Implement this method!
  // Hint: Iterate the version_lists and return the verion whose write timestamp
  // (version_id) is the largest write timestamp less than or equal to txn_unique_id.
  if(!mvcc_data_.count(key)){
    return false;
  }

  deque<Version*> *data = mvcc_data_[key];
  if(data->empty()){
    return false;
  } 
  else{
    int maxlessthan = 0;
    deque<Version*> version_list = *data;
    for (deque<Version*>::iterator itr = version_list.begin(); itr != version_list.end(); itr++){
        Version* version = *itr;
        //get largest less than version
        if (version->version_id_ <= txn_unique_id && version->version_id_ > maxlessthan){
          //set new version
          *result = version->value_;
          version->max_read_id_ = txn_unique_id;
        }  
    }
  }
  return true;
}


// Check whether apply or abort the write
bool MVCCStorage::CheckWrite(Key key, int txn_unique_id) {
  // CPSC 438/538:
  //
  // Implement this method!
  if(!mvcc_data_.count(key)){
    return false;
  }

  deque<Version*> *data= mvcc_data_[key];

  if(data->empty()){
    return true;
  } 
  else{
    deque<Version*> version_list = *data;
    Version* version = version_list.back();

    if(version->max_read_id_ > txn_unique_id){
      return false;
    }
  }
  // Hint: Before all writes are applied, we need to make sure that each write
  // can be safely applied based on MVCC timestamp ordering protocol. This method
  // only checks one key, so you should call this method for each key in the
  // write_set. Return true if this key passes the check, return false if not. 
  // Note that you don't have to call Lock(key) in this method, just
  // call Lock(key) before you call this method and call Unlock(key) afterward.
  
  
  return true;
}

// MVCC Write, call this method only if CheckWrite return true.
void MVCCStorage::Write(Key key, Value value, int txn_unique_id) {
  // CPSC 438/538:
  //
  // Implement this method!
  Version* new_update = new Version();
  
  new_update->value_ = value;
  new_update->version_id_ = txn_unique_id;
  new_update->max_read_id_ = 0;

  deque<Version*> versionX;
  if(mvcc_data_.count(key)){
    versionX = mvcc_data_[key];
  }
  else{
    versionX = new deque<Version*>;
    mvcc_data_[key] = versionX;
  }

  versionX->push_front(new_update);
  // Hint: Insert a new version (malloc a Version and specify its value/version_id/max_read_id)
  // into the version_lists. Note that InitStorage() also calls this method to init storage. 
  // Note that you don't have to call Lock(key) in this method, just
  // call Lock(key) before you call this method and call Unlock(key) afterward.
}


