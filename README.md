# EllieScript

### A TypeSafe JavaScript dialect, that transpiles to pure JS and runs ontop of QuickJS by Fabrice Bellard.

```js
const sum_of_add Number = add(200, 23);

console.warn(":>>", sum_of_add);

fn add(a Number, b Number) Number {
    return a + b;
}

fn make_array() Array<String> {
    const arr Array<String> = [ "any", "string" ];

    return Array<String> [ "Hello", "World" ];
}
```

# TODO

- [X] Global console object. 
- [ ] Global Number constructor.
- [ ] Global BigInt constructor.
- [ ] Global String constructor.
- [ ] Global Boolean constructor.
- [ ] Array
  - [X] Array.push 
  - [X] Array.pop
  - [X] Array.shift
  - [X] Array.unshift
- [ ] Global Date constructor.