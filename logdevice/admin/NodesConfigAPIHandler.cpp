/**
 * Copyright (c) 2017-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "logdevice/admin/NodesConfigAPIHandler.h"
#include "logdevice/admin/AdminAPIUtils.h"
#include "logdevice/common/Worker.h"
#include "logdevice/common/configuration/Configuration.h"
#include "logdevice/common/request_util.h"
#include "logdevice/common/settings/SettingsUpdater.h"
#include "logdevice/server/Server.h"
#include "logdevice/server/ServerProcessor.h"

using namespace facebook::logdevice;

namespace facebook { namespace logdevice {

void NodesConfigAPIHandler::getNodesConfig(
    thrift::NodesConfigResponse& out,
    std::unique_ptr<thrift::NodesFilter> filter) {
  // The idea is that we keep filtering the nodes based on the supplied filter
  // until we get a final list to return.

  auto server_config = processor_->config_->getServerConfig();
  std::vector<thrift::NodeConfig> result_nodes;

  forFilteredNodes(
      server_config->getNodes(),
      filter.get(),
      [&](const std::pair<const node_index_t, configuration::Node>& it) {
        thrift::NodeConfig node;
        fillNodeConfig(node, it.first, it.second);
        result_nodes.push_back(std::move(node));
      });
  out.set_nodes(std::move(result_nodes));
  out.set_version(static_cast<int64_t>(server_config->getVersion().val()));
}

}} // namespace facebook::logdevice
