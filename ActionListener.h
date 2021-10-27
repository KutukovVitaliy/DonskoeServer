//
// Created by kvitadmin on 27.10.2021.
//

#ifndef DONSKOESERVER_ACTIONLISTENER_H
#define DONSKOESERVER_ACTIONLISTENER_H
#include "mqtt/async_client.h"

// Callbacks for the success or failures of requested actions.
// This could be used to initiate further action, but here we just log the
// results to the console.
class ActionListener : public virtual mqtt::iaction_listener{
    std::string name_;

    void on_failure(const mqtt::token& tok) override;
    void on_success(const mqtt::token& tok) override;

public:
    ActionListener(const std::string& name) : name_(name) {}
};


#endif //DONSKOESERVER_ACTIONLISTENER_H
