# /usr/bin/env python3

# Simple Python binding for a stack of integer values.

from ctypes import c_int, c_ulong, CDLL

try:
    dso = CDLL("./libstackpool.so")
    dso.stack.restype
except:
    print('You should compile the C interface before (command "make").')
    exit(1)

# stack
dso.stack.restype = c_ulong
# push
dso.push.argtypes = [c_int, c_ulong]
dso.push.restype = c_ulong
# pop
dso.pop.argtypes = [c_ulong]
dso.pop.restype = c_ulong
# value
dso.pop.argtypes = [c_ulong]
dso.pop.restype = int
# size
dso.size.argtypes = [c_ulong]
dso.size.restype = int

dso.empty_pool()

head1 = dso.stack()
head1 = dso.push(10, head1)
head1 = dso.push(100, head1)
head1 = dso.push(1000, head1)

head2 = dso.stack()
head2 = dso.push(2, head2)
head2 = dso.push(4, head2)
head2 = dso.push(8, head2)
head2 = dso.push(16, head2)

def unroll_stack(head):
    s = dso.size(head)
    print('The given stack has size {}'.format(s))
    for i in range(s):
        print(dso.value(head))
        head = dso.pop(head)

print('This is stack1')
unroll_stack(head1)

print('This is stack2')
unroll_stack(head2)

print('This is stack3')
unroll_stack(dso.stack())

dso.delete_pool()
