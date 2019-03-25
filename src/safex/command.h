//
// Created by amarko on 4.3.19..
//

#ifndef SAFEX_COMMAND_H
#define SAFEX_COMMAND_H

#include <string>
#include <exception>
#include <vector>
#include <iostream>

#include "crypto/crypto.h"
#include "crypto/hash.h"
#include "cryptonote_core/blockchain.h"

#include "storages/portable_storage.h"
#include "safex_core.h"

#include "misc_log_ex.h"


namespace safex
{

  /* Binary storage fields */
  static const std::string FIELD_VERSION = "version";
  static const std::string FIELD_COMMAND = "command";
  static const std::string FIELD_LOCK_TOKEN_AMOUNT = "lock_token_amount";
  static const std::string FIELD_LOCKED_TOKEN_OUTPUT_INDEX = "locked_token_output_index";

  struct token_lock_result
  {
    uint64_t token_amount; //locked amount
    uint32_t block_number; //block where it is locked

    bool valid;
  };

  struct token_lock_data
  {
    uint32_t reserved;
  };

  struct token_unlock_result
  {
    uint64_t token_amount; //unlocked token amount
    uint64_t interest; //collected interest from network fees over period
    uint32_t block_number; //block where it is unlocked

    bool valid;
  };

  struct token_collect_result
  {
    uint64_t token_amount; //amount of tokens that is relocked
    uint64_t interest; //collected interest from network fees over period
    uint32_t block_number; //block where it is unlocked

    bool valid;
  };


  /**
  * @brief script command representation
  *
  * Safex Command protocol is intended to expand functionality
  * of the blockchain and to enable easy addition of the new features
  * without having to make significant changes
  * to the current blockchain core protocol.
  */
  template<typename CommandResult>
  class command
  {
    public:


      friend class safex_command_serializer;


      /**
       * @param _version Safex command protocol version
       * @param _command_type actuall command, like token lock
       *
      * */
      command(const uint32_t _version, const command_t _command_type) : version(_version), command_type(_command_type)
      {
        SAFEX_COMMAND_CHECK_AND_ASSERT_THROW_MES((_command_type < command_t::invalid_command), "Invalid command type", _command_type);
        SAFEX_COMMAND_CHECK_AND_ASSERT_THROW_MES((_version <= SAFEX_COMMAND_PROTOCOL_VERSION), "Unsupported command protocol version " + std::to_string(_version), command_type);

      }

      virtual bool execute(const cryptonote::BlockchainDB &blokchain, const cryptonote::txin_to_script &txin, CommandResult &cr) = 0;

      uint32_t getVersion() const
      { return version; }

      command_t get_command_type() const
      { return command_type; }

      virtual ~command() = default;


    protected:

      virtual bool store(epee::serialization::portable_storage &ps) const;

      virtual bool load(epee::serialization::portable_storage &ps);

      uint32_t version;
      command_t command_type;

    protected:

  };


  //Token lock command
  class token_lock : public command<token_lock_result>
  {
    public:

      friend class safex_command_serializer;

      /**
       * @param _version Safex command protocol version
       * @param _token_amount amount of tokens to lock
       *
      * */
      token_lock(const uint32_t _version, const uint64_t _token_amount) : command<token_lock_result>(_version, command_t::token_lock), lock_token_amount(_token_amount)
      {

      }

      token_lock() : command<token_lock_result>(0, command_t::token_lock), lock_token_amount(0)
      {

      }

      uint64_t get_lock_token_amount() const
      { return lock_token_amount; }

      virtual bool execute(const cryptonote::BlockchainDB &blokchain, const cryptonote::txin_to_script &txin, token_lock_result &cr) override;

    protected:

      virtual bool store(epee::serialization::portable_storage &ps) const override;

      virtual bool load(epee::serialization::portable_storage &ps) override;

      uint64_t lock_token_amount;
  };


  //Token unlock command
  class token_unlock : public command<token_unlock_result>
  {
    public:

      friend class safex_command_serializer;

      /**
       * @param _version Safex command protocol version
       * @param _locked_token_output_index global index of txout_to_script output that is being unlocked
       *
      * */
      token_unlock(const uint32_t _version, const uint64_t _locked_token_output_index) : command<token_unlock_result>(_version, command_t::token_unlock),
              locked_token_output_index(_locked_token_output_index)
      {

      }

      token_unlock() : command<token_unlock_result>(0, command_t::token_unlock), locked_token_output_index(0)
      {

      }

      uint64_t get_locked_token_output_index() const
      { return locked_token_output_index; }

      virtual bool execute(const cryptonote::BlockchainDB &blokchain, const cryptonote::txin_to_script &txin, token_unlock_result &cr) override;

    protected:

      virtual bool store(epee::serialization::portable_storage &ps) const override;

      virtual bool load(epee::serialization::portable_storage &ps) override;

      uint64_t locked_token_output_index;
  };


  //Token collect command
  class token_collect : public command<token_collect_result>
  {
    public:

      friend class safex_command_serializer;

      /**
       * @param _version Safex command protocol version
       * @param _locked_token_output_index global index of txout_to_script output that is being unlocked
       *
      * */
      token_collect(const uint32_t _version, const uint64_t _locked_token_output_index) : command<token_collect_result>(_version, command_t::token_collect),
                                                                                          locked_token_output_index(_locked_token_output_index)
      {

      }

      token_collect() : command<token_collect_result>(0, command_t::token_collect), locked_token_output_index(0)
      {

      }

      uint64_t get_locked_token_output_index() const
      { return locked_token_output_index; }

      virtual bool execute(const cryptonote::BlockchainDB &blokchain, const cryptonote::txin_to_script &txin, token_collect_result &cr) override;

    protected:

      virtual bool store(epee::serialization::portable_storage &ps) const override;

      virtual bool load(epee::serialization::portable_storage &ps) override;

      uint64_t locked_token_output_index;
  };


  class safex_command_serializer
  {
    public:

      template<typename Command>
      static bool store_command(const Command &com, std::vector<uint8_t> &target)
      {
        epee::serialization::portable_storage ps = AUTO_VAL_INIT(ps);

        //here serialize particular
        com.store(ps);

        epee::serialization::binarybuffer bin_target = AUTO_VAL_INIT(bin_target);

        if (!ps.store_to_binary(bin_target))
        {
          throw safex::command_exception(com.get_command_type(), "Could not store to portable storage binary blob");
        }

        target.clear();
        target = std::vector<uint8_t>(bin_target.begin(), bin_target.end());

        return true;

      }

      template<typename Command>
      static bool load_command(const std::vector<uint8_t> &source, Command &com)
      {
        const epee::serialization::binarybuffer bin_source(source.begin(), source.end());
        epee::serialization::portable_storage ps = AUTO_VAL_INIT(ps);
        if (!ps.load_from_binary(bin_source))
        {
          throw safex::command_exception(command_t::invalid_command, "Could not load portable storage from binary blob");
        }

        com.load(ps);

        return true;

      }


      static command_t get_command_type(const std::vector<uint8_t> &source)
      {
        const epee::serialization::binarybuffer bin_source(source.begin(), source.end());
        epee::serialization::portable_storage ps = AUTO_VAL_INIT(ps);
        if (!ps.load_from_binary(bin_source))
        {
          throw safex::command_exception(command_t::nop, "Could not load portable storage from binary blob");
        }

        uint32_t command_type = 0;
        ps.get_value(FIELD_COMMAND, command_type, nullptr);

        return static_cast<command_t>(command_type);
      }

      /**
       *
       * @tparam Data POD data structure
       * @param data structure holds advanced utxo data
       * @param target vector of bytes of serialized data
       * @return true if succede
       */
      template<typename Data>
      static bool store_data(const Data& data, std::vector<uint8_t> &target)
      {
        epee::serialization::portable_storage ps = AUTO_VAL_INIT(ps);

        //here serialize particular
        data.store(ps);

        epee::serialization::binarybuffer bin_target = AUTO_VAL_INIT(bin_target);

        if (!ps.store_to_binary(bin_target))
        {
          throw safex::command_exception(command_t::invalid_command, "Could not store data to portable storage binary blob");
        }

        target.clear();
        target = std::vector<uint8_t>(bin_target.begin(), bin_target.end());

        return true;
      }

      template<typename Data>
      static bool load_data(const std::vector<uint8_t> source, Data &data)
      {
        const epee::serialization::binarybuffer bin_source(source.begin(), source.end());
        epee::serialization::portable_storage ps = AUTO_VAL_INIT(ps);
        if (!ps.load_from_binary(bin_source))
        {
          throw safex::command_exception(command_t::invalid_command, "Could not load portable storage data from binary blob");
        }

        data.load(ps);

        return true;
      }

  };


} //namespace safex


#endif //SAFEX_COMMAND_H
