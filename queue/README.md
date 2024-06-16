# Queue
## Description
This module lets users join a queue. IRCOps with operclass permission `queue` will be able to view this list and have access to a second command which will begin a conversation between the queuer and the staff member.

Also provides RPC method `queue.list` to fetch a list of users in the queue.
## `/QUEUE` Syntax
### Join the queue:
`/QUEUE`
### View the size of the current queue:
`/QUEUE size`
### View your position in the queue:
`/QUEUE position`
### Leave the queue
`/QUEUE leave`
### View the list of people in the queue (Oper Only)
`/QUEUE list`
## Begin a conversation with the person at the front of the queue (Oper Only)
`/NEXTQUEUE`
