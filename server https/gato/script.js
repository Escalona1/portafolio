document.addEventListener("mousemove", function(event) {
    var cat = document.getElementById("cat");
    cat.style.left = event.clientX + "px";
    cat.style.top = event.clientY + "px";
});
