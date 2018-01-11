## Cases ##
-----------

#### Dynamic Expressions - runtimex.cpp ####
The expression tree is built at run time. We avoid the construction of temporaries but we have multiple function calls  
for each vector element we want to calculate. This is still a major performance drawback.  

#### Expression Templates ####
The promise here is that the evaluation function is inlined in the initial expression ie:
```c
ret = a * b + c
```
will inline the whole right-hand side in the loop that populates ret with values.  
  
#### Move Semantics ###
An option (that requires testing though) is to define everything as assign-add operations over the rvalue refs, eg:  
```c
Vector &&operator +(Vector &&l_, const Vector &r_) { 
    for (size_t i = 0; i < l_.size(); ++i)
        l_[i] += r_[i];
    return l_; 
}
```
The intermediate result is accumulated in the left rvalue argument!  
  

