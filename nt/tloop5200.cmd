# test loop with proc inside
cls
print("--- tloop52.cmd")

function factorial_tail(n, acc){
    if (n <= 1) return acc
    # This is a tail call! The result is returned directly with no pending operations.
    return factorial_tail(n - 1, n * acc)
}

a=factorial_tail(6,1):   720 | 0x2D0 | 0o1320

