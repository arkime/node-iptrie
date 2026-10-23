var assert = require('assert'),
    path = require('path'),
    { Worker } = require('worker_threads'),
    iptrie = require('../index');

var trie = new iptrie.IPTrie();

assert.throws(function () { iptrie.IPTrie(); }, TypeError);

assert.throws(function () { trie.add('1.2.3.4', 33, 'x'); }, RangeError);
assert.throws(function () { trie.add('::1', 129, 'x'); }, RangeError);
assert.throws(function () { trie.add('1.2.3.4', 288, 'x'); }, RangeError);
assert.throws(function () { trie.add('1.2.3.4', -1, 'x'); }, RangeError);
assert.throws(function () { trie.del('1.2.3.4', 33); }, RangeError);
assert.throws(function () { trie.add('nope', 8, 'x'); }, /Could not parse IP/);
assert.throws(function () { trie.add(); }, /First argument must be an IP/);

assert.throws(function () { iptrie.IPTrie.prototype.find.call({}, '1.2.3.4'); }, TypeError);

trie.add('10.0.0.0', 8, 'first');
trie.add('10.0.0.0', 8, 'second');
assert.equal(trie.find('10.1.2.3'), 'second');
assert.equal(trie.del('10.0.0.0', 8), true);
assert.equal(trie.find('10.1.2.3'), undefined);

var worker = new Worker(
  "var t = new (require(" + JSON.stringify(path.join(__dirname, '..', 'index')) + ").IPTrie)();" +
  "t.add('192.168.0.0', 16, 'w');" +
  "require('worker_threads').parentPort.postMessage(t.find('192.168.1.1'));",
  { eval: true });
worker.on('message', function (msg) {
  assert.equal(msg, 'w');
  console.log('validate ok');
});
worker.on('error', function (err) { throw err; });
