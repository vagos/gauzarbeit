# Janet initialization script
# This file is loaded when the Janet scripting system is initialized

# Test basic Janet functionality
(def greeting "Janet scripting system loaded!")
(def test-calculation (+ 2 3 (* 4 5)))

# Define some helper functions for things
(defn thing-info [thing]
  "Thing information will go here")

# Show that Janet code is executing
(def pi-approx (/ 22 7))