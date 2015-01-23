function gamma(x) {
  return parseInt(255 * Math.pow(x/255.0, 1/0.45));
}

function sine(x) {
  return parseInt(Math.sin(x*Math.PI/255.0)*255)
}

for(var x = 0; x < 256; x++) {
  console.log(gamma(sine(x)));
}
