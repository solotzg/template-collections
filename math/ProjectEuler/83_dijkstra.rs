// extern crate mio;

// use mio::*;
// use mio::tcp::{TcpListener, TcpStream};

// // Setup some tokens to allow us to identify which event is
// // for which socket.
// const SERVER: Token = Token(0);
// const CLIENT: Token = Token(1);

// fn main() {
//     let addr = "127.0.0.1:13265".parse().unwrap();

//     // Setup the server socket
//     let server = TcpListener::bind(&addr).unwrap();

//     // Create a poll instance
//     let poll = Poll::new().unwrap();

//     // Start listening for incoming connections
//     poll.register(&server, SERVER, Ready::readable(), PollOpt::edge())
//         .unwrap();

//     // Setup the client socket
//     let sock = TcpStream::connect(&addr).unwrap();

//     // Register the socket
//     poll.register(&sock, CLIENT, Ready::readable(), PollOpt::edge())
//         .unwrap();

//     // Create storage for events
//     let mut events = Events::with_capacity(1024);

//     loop {
//         poll.poll(&mut events, None).unwrap();

//         for event in events.iter() {
//             match event.token() {
//                 SERVER => {
//                     // Accept and drop the socket immediately, this will close
//                     // the socket and notify the client of the EOF.
//                     let _ = server.accept();
//                 }
//                 CLIENT => {
//                     // The server just shuts down the socket, let's just exit
//                     // from our event loop.
//                     return;
//                 }
//                 _ => unreachable!(),
//             }
//         }
//     }
// }


extern crate typed_arena;
extern crate rayon;

use typed_arena::Arena;
use rayon::prelude::*;
use std::cmp;
use std::f64;
// use std::collections::HashMap;
use std::collections::BinaryHeap;
use std::cmp::Ordering;
use std::fs::File;
use std::io::prelude::*;
use std::io::BufReader;

#[derive(Copy, Clone, Eq, PartialEq)]
struct State {
    cost: i32,
    pos: usize,
}

impl Ord for State {
    fn cmp(&self, other: &Self) -> Ordering {
        other
            .cost
            .cmp(&self.cost)
            .then_with(|| self.pos.cmp(&other.pos))
    }
}

impl PartialOrd for State {
    fn partial_cmp(&self, other: &State) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

fn dijkstra(mmap: &Vec<Vec<(usize, i32)>>, src: usize, tar: usize) -> i32 {
    let num = mmap.len();
    let mut ans = vec![i32::max_value()/3; num];
    ans[src] = 0i32;
    let mut bh = BinaryHeap::<State>::new();
    bh.push(State { cost: 0, pos: src });
    while !bh.is_empty() {
        let s = bh.pop().unwrap();
        if s.cost != ans[s.pos] {
            continue;
        }
        for p in &mmap[s.pos] {
            if p.1 + s.cost < ans[p.0] {
                ans[p.0] = p.1 + s.cost;
                bh.push(State {
                            cost: ans[p.0],
                            pos: p.0,
                        });
            }
        }
    }
    return ans[tar];
}

fn go(n: usize) -> Vec<Vec<i32>> {
    let fname = "C:/Users/tzgn5892/Downloads/p083_matrix.txt";
    let file = File::open(fname).unwrap();
    let fin = BufReader::new(file);
    let mut data = vec![Vec::<i32>::new(); n];
    let mut u = 0usize;
    for line in fin.lines() {
        let s = line.unwrap().clone();
        let ds: Vec<&str> = s.split(',').collect();
        for v in 0..ds.len() {
            data[u].push(ds[v].parse::<i32>().unwrap());
        }
        u += 1;
    }
    data
}

fn main() {
    let n = 80usize;
    let m = 80usize;
    let tol = m * n;
    let dir = [[0, 1], [0, -1], [1, 0], [-1, 0]];
    let data = go(n);
    let mut mmap = vec![Vec::<(usize,i32)>::new(); tol];
    for x in 0..n {
        for y in 0..m {
            let o = x * m + y;
            for d in &dir {
                let xx = d[0] + (x as i32);
                let yy = d[1] + (y as i32);
                if xx >= 0 && xx < (n as i32) && yy >= 0 && yy < (m as i32) {
                    let p = (xx as usize) * m + (yy as usize);
                    mmap[o].push((p, data[xx as usize][yy as usize]));
                }
            }
        }
    }
    println!("{}", dijkstra(&mmap, 0, tol - 1) + data[0][0]);
}
