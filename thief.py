#!/usr/bin/env python3
# Steals CTM's assets :)
# GPL.
from typing import Callable, Tuple
def split_before(c: str, p: Callable) -> Tuple[str, str]:
    i = 0
    for i, v in enumerate(c):
        if p(v):
            break
    else:
        # Nothing fulfills the predicate.
        # Return before = full string, after = empty
        return c, ''
    return c[:i], c[i:]

def flip(a, b):
    return b, a

def consume_string(s: str):
    return split_before(s, lambda c: c in "|() /=")

def consume_quoted(s: str):
    res = str()
    initial = s
    c1 = s[0]
    s = s[1:]
    while s:
        a, b = split_before(s, lambda c: c == c1)
        if a and a[-1] == '\\' and b:
            # ok I could parse this properly but like
            # why
            # so we'll just do this and if it's an issue..? eh
            res += a[0:-1] + b[0]
            s = b[1:]
            continue
        else:
            res += a
            s = b
            break
    if not s:
        raise RuntimeError(f'Could not find end to string: {initial}')
    return res, s[1:]

def to_constexpr(d: dict[str, str]):
    item = 'constexpr Advancement {}|| "{}", "{}", "{}", "{}", "{}", ||;\n'
    id_ = d.get('id', '{}')
    name_ = d.get('name', '{}')
    icon_ = d.get('icon', '{}')
    type_ = d.get('type', '{}')
    short_name_ = d.get('short_name', '{}')
    nom = id_.rsplit('/', 1)[1]
    return item.format(nom, id_, name_, icon_, type_, short_name_, ).replace('||', '{', 1).replace('||', '}', 1).replace('"{}"', '{}')

def thievery(root):
    l = ['adventure', 'end', 'husbandry', 'nether', 'minecraft']
    src = open("include/.advancements.hpp").read()
    of = open("include/advancements.hpp", "w")
    of.write(src)
    for y in l:
        with open(f"{root}/AATool/assets/objectives/1.16/advancements/{y}.xml") as file:
            loc = [x.strip() for x in file.readlines()]
        for ln in loc:
            if ln.startswith('<advancement'):
                _, ln = ln.split(' ', 1)
                d = {}
                while True:
                    ln = ln.strip()
                    if not ln or not ln[0].isalpha():
                        break
                    k, ln = consume_string(ln)
                    if ln.startswith('='):
                        v, ln = consume_quoted(ln[1:])
                        d[k] = v
                        continue
                    raise RuntimeError('idk' + ln)
                of.write(to_constexpr(d))


if __name__ == "__main__":
    import sys
    root = sys.argv[1]
    thievery(root)
