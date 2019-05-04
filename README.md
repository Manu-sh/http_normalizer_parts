# http_normalizer
###### http url normalizer for crawlers

This library is intented to produce normalized http url and not uri, for example http url dosn't have an userinfo part (https://tools.ietf.org/html/rfc7230).

#### structure of http urls

```
http-URI = "http:" "//" authority path-abempty [ "?" query ] [ "#" fragment ]
```

#### http_parts
no regex are used here

###### normalize_protocol()
* Decoding percent-encoded octets of unreserved characters
* Converting the scheme to lower case
* Then check if is an http/s scheme

###### normalize_hostname()
* Decoding percent-encoded octets of unreserved characters
* Empty labels are not allowed
* Max 64 chars are allowed
* Require at least 2 labels (so host like localhost are rejected)
* The maximum label are 20
* each labels must start and end with `[[:alnum:]]` `'-'` is allowed in the middle
* Converting the host to lower case
* Remove all www. labels from the start of string (these are counted as part of  max_labels) so `www.www.google.com` become `google.com`

###### normalize_port()
* Check if is a the port is between (1-MAX_USHORT)
* if is a valid port and the port can be removed (is the default port of protocol, for example 443 for https) return 0, in case of error return -1
or the port itself if is valid

###### normalize_path()
* Decoding percent-encoded octets of unreserved characters
* Encoding reserved characters
* Removing dot-segments
* Removing double slashes
* Removing default directory index (ex. `/a/index.html`  become `a/index.html`)

###### normalize_query()
* Decoding percent-encoded octets of unreserved characters
* Drop '=' in Query Parameter if Empty (ex. `q=&x` become `q&x`)
* Encoding reserved characters
* Space becomes plus sign In query (ex. `q=%20` become `q=+`)
* Sorted query parameters by key (ex. `q=10&q=20&a=1` become `a=1&q=20`)

###### Copyright © 2019, [Manu-sh](https://github.com/Manu-sh), s3gmentationfault@gmail.com. Released under the [MIT license](LICENSE).