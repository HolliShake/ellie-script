# EllieScript

### A TypeSafe JavaScript dialect, that transpiles to pure JS and runs ontop of QuickJS by Fabrice Bellard.

```js
fn mul(a:int, b:int):int {
    return a * b;
}

"Yeha" == "Yeha";

if (3 > 2) 
    100 << 100;
else 
    3 > 2;

var sum:number = 2 + 2, dif:string = ("Hola" + "World!");

const x:int = 100n, y:number = sum;

console.log(1, 2, 3, add(20, 35), 20 + 32);
console.warn;
console.error;
mul(1n, 1000n);

fn factorial(n:number):number {
    let x:number = 2, y:string = "Yellow";
    if (n <= 1)
        return 1;

    return n * factorial(n - 1);
}

fn foo():void {
    return null;
}

console.log(factorial(5));
console.error(factorial(5));

if (3 > 2) {
    const x:int = 10n;
}

pub async fn get_name():string {
    return "";
}

pub fn exp():number {
    while (3 > 2) {
        return 1;
    }
    {
        {
            return 1;
        }
    }
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