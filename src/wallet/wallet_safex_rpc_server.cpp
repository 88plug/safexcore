#include <boost/format.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdint>
#include "include_base_utils.h"
using namespace epee;

#include "wallet_rpc_server.h"
#include "wallet/wallet_args.h"
#include "common/command_line.h"
#include "common/i18n.h"
#include "cryptonote_config.h"
#include "cryptonote_basic/cryptonote_format_utils.h"
#include "cryptonote_basic/account.h"
#include "wallet_rpc_server_commands_defs.h"
#include "misc_language.h"
#include "string_coding.h"
#include "string_tools.h"
#include "crypto/hash.h"
#include "mnemonics/electrum-words.h"
#include "rpc/rpc_args.h"
#include "rpc/core_rpc_server_commands_defs.h"

#undef SAFEX_DEFAULT_LOG_CATEGORY
#define SAFEX_DEFAULT_LOG_CATEGORY "wallet.rpc"


namespace tools {
   
    bool wallet_rpc_server::on_unstake_token(const wallet_rpc::COMMAND_RPC_UNSTAKE_TOKEN::request& req, wallet_rpc::COMMAND_RPC_UNSTAKE_TOKEN::response& res, epee::json_rpc::error& er)
    {
        return false;
    }
    bool wallet_rpc_server::on_donate_safex_fee(const wallet_rpc::COMMAND_RPC_DONATE_SAFEX_FEE::request& req, wallet_rpc::COMMAND_RPC_DONATE_SAFEX_FEE::response& res, epee::json_rpc::error& er)
    {
        return false;
    }
    bool wallet_rpc_server::on_make_demo_purchase(const wallet_rpc::COMMAND_RPC_DEMO_PURCHASE::request& req, wallet_rpc::COMMAND_RPC_DEMO_PURCHASE::response& res, epee::json_rpc::error& er)
    {
        return false;
    }
    bool wallet_rpc_server::on_get_demo_offers(const wallet_rpc::COMMAND_RPC_GET_DEMO_OFFERS::request& req, wallet_rpc::COMMAND_RPC_GET_DEMO_OFFERS::response& res, epee::json_rpc::error& er)
    {
        return false;
    }
}
