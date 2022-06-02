# Centaur

> What is Centaur?

Centaur is a GUI application based on Qt, in order to display Stock markets Information and Cryptocurrencies, it allows
creating trading strategies, connecting with exchanges (Via a Server/Client model and Qt plugins).

Right now, Centaur is a set of applications and libraries in order to perform all the features intended.

### Why Qt and C++?

Because I like C++, and I wanted this piece of software to be platform independent as much as possible and Qt was by far
the best option. Qt Quick (QML) was not an option because this software is intended to be use in Desktop platforms not
in mobile devices.

There is possibility of using Python, Java, Javascript or any other language that supports the WebSockets protocol.
See [Why Server/Client Model?](readme.md)

### Motivation

I think a free, feature-complete Desktop Application for visualize and analyze markets data as well, as perform some
trading operations is not yet in the near future. So, Why not writing one?

As of now, Centaur must not be cataloged as a feature-complete software, but I wanted to be. So, anyone wanting to
contribute, give some ideas or give better ideas than mine are welcome.

### Server/Client Model

Client applications can be trading bots handling accounts interoperating with the main application and the plugins.

For example, a client app can be handling orders, receiving input about the user balances and informing this to the main
application, meanwhile, a local plugin can be accessing the candlestick information and the orderbook to create the
depth chart in the main interface

### Plugins

### Why?

> Why Server/Client Model?

Actually, I already have a console C++ based application that do automatic trading based on scalping techniques, and I
wanted to add a GUI in order to easily visualize what is going on. However, adding the extra overhead of a GUI to a
scalping software was not ideal.

So the best way I came up without the need to add the GUI overhead to my console app or reimplementing the strategy as a
Qt pluing was a basic Server/Client using WebSockets to connect the trading logic to the GUI using a JSON-based
communication protocol.

This Server/client model opened the door to use Python to implement a basic Sentiment Analysis of stock/crypto markets.
See [Sentiment Analysis](san/readme.md). Also it opens the possibility of running the automatic trading bot in a
specialized server and visualize the data in my laptop remotely.

See [Protecols](contributing/protocols.md) to implement a trading strategy and visualize it on the GUI.

Currently, there is a set of client applications for connecting to the exchanges

> Client Applications

The Binance Exchange Client Application uses the [BinanceAPI](binanceAPI) that I wrote for my trading Bot, and
implements the Exchange User WebSockets, Market WebSockets as well as some basic User account data visualization.

## Features

- [ ] Binance connection
- [ ] Gate.IO connection
- [ ] Bybit connection
- [ ] Stocks markets
- [ ] Sentiment analysis of basic stock/crypto markets (Python Implemented)

## Compiling

> You can compile via cmake/make. If you encounter issues with the cmake files let me know.

### Dependencies

- Qt6
- {fmt}
- libWebSockets
- OpenSSL
- rapidJSON
- curl
- CPR.
- ZLIB

### Installing

You can download the binaries for macOS Intel/arm, Windows, Linux, and follow the installation instructions.