import dist.pwnedlookup as pl

def assert_exception(fn):
    try: fn()
    except: return
    raise Exception("Function did not throw")

files = ['data/pwned-sha1', 'data/pwned-count']
assert pl.sha1_leak_count('0000000000000000000000000000000000000000', *files) == 0
assert pl.sha1_leak_count('000016ccA44A642D48A4DDB7E87FA0C5CEC7CF28', *files) == 2
assert pl.sha1_leak_count('00000000DD7F2A1C68A35673713783CA390C9E93', *files) == 630
[ assert_exception(fn) for fn in [
    lambda: pl.sha1_leak_count('012345678901234567890123456789012345678', *files),
    lambda: pl.sha1_leak_count('0123456789012345678901234567890123456789'),
    lambda: pl.sha1_leak_count('0123456789012345678901234567890123456789', '', ''),
]]
assert pl.password_leak_count('password', *files) == 3_861_493
assert pl.password_leak_count('password123', *files) == 126_927
[ assert_exception(fn) for fn in [
    lambda: pl.password_leak_count('password'),
    lambda: pl.password_leak_count('password123', '', ''),
]]