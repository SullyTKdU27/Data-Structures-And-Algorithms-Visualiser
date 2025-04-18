import React, { useState, useEffect, useRef } from 'react';

// Main App Component
export default function AlgorithmVisualizer() {
  const [array, setArray] = useState([]);
  const [sortingAlgorithm, setSortingAlgorithm] = useState('bubbleSort');
  const [isSorting, setIsSorting] = useState(false);
  const [speed, setSpeed] = useState(50);
  const [arraySize, setArraySize] = useState(30);
  const [currentStep, setCurrentStep] = useState(0);
  const [totalSteps, setTotalSteps] = useState(0);
  const [comparisons, setComparisons] = useState(0);
  const [swaps, setSwaps] = useState(0);
  const [animationQueue, setAnimationQueue] = useState([]);
  const [currentIndices, setCurrentIndices] = useState([]);
  const [sortedIndices, setSortedIndices] = useState([]);

  // References for animation control
  const requestRef = useRef(null);
  const animationQueueRef = useRef([]);
  const isPlayingRef = useRef(false);

  // Generate random array
  const generateArray = () => {
    const newArray = [];
    for (let i = 0; i < arraySize; i++) {
      newArray.push(Math.floor(Math.random() * 100) + 5);
    }
    setArray(newArray);
    setAnimationQueue([]);
    setCurrentIndices([]);
    setSortedIndices([]);
    setCurrentStep(0);
    setTotalSteps(0);
    setComparisons(0);
    setSwaps(0);
  };

  // Initial array generation
  useEffect(() => {
    generateArray();
  }, [arraySize]);

  // Handle algorithm selection
  const handleAlgorithmChange = (e) => {
    setSortingAlgorithm(e.target.value);
    stopSorting();
  };

  // Handle array size change
  const handleSizeChange = (e) => {
    setArraySize(Number(e.target.value));
    stopSorting();
  };

  // Handle speed change
  const handleSpeedChange = (e) => {
    setSpeed(Number(e.target.value));
  };

  // Stop sorting animation
  const stopSorting = () => {
    if (requestRef.current) {
      cancelAnimationFrame(requestRef.current);
      requestRef.current = null;
    }
    isPlayingRef.current = false;
    setIsSorting(false);
  };

  // Reset array
  const resetArray = () => {
    stopSorting();
    generateArray();
  };

  // Process the sorting algorithms
  const processAlgorithm = (algoName) => {
    const newArray = [...array];
    let animations = [];
    let comparisonsCount = 0;
    let swapsCount = 0;

    const compare = (i, j) => {
      comparisonsCount++;
      animations.push({ type: 'compare', indices: [i, j] });
      return newArray[i] - newArray[j];
    };

    const swap = (i, j) => {
      swapsCount++;
      animations.push({ type: 'swap', indices: [i, j] });
      const temp = newArray[i];
      newArray[i] = newArray[j];
      newArray[j] = temp;
    };

    const markSorted = (index) => {
      animations.push({ type: 'sorted', index });
    };

    // Implement algorithms
    switch (algoName) {
      case 'bubbleSort': {
        for (let i = 0; i < newArray.length; i++) {
          for (let j = 0; j < newArray.length - i - 1; j++) {
            if (compare(j, j + 1) > 0) {
              swap(j, j + 1);
            }
          }
          markSorted(newArray.length - i - 1);
        }
        break;
      }
      case 'selectionSort': {
        for (let i = 0; i < newArray.length; i++) {
          let minIdx = i;
          for (let j = i + 1; j < newArray.length; j++) {
            if (compare(j, minIdx) < 0) {
              minIdx = j;
            }
          }
          if (minIdx !== i) {
            swap(i, minIdx);
          }
          markSorted(i);
        }
        break;
      }
      case 'insertionSort': {
        for (let i = 1; i < newArray.length; i++) {
          let j = i;
          while (j > 0 && compare(j, j - 1) < 0) {
            swap(j, j - 1);
            j--;
          }
          markSorted(i);
        }
        break;
      }
      case 'quickSort': {
        const quickSortHelper = (start, end) => {
          if (start >= end) {
            if (start === end) markSorted(start);
            return;
          }
          
          let pivot = end;
          let i = start - 1;
          
          for (let j = start; j < end; j++) {
            if (compare(j, pivot) <= 0) {
              i++;
              if (i !== j) {
                swap(i, j);
              }
            }
          }
          
          swap(i + 1, end);
          
          const pivotIndex = i + 1;
          markSorted(pivotIndex);
          
          quickSortHelper(start, pivotIndex - 1);
          quickSortHelper(pivotIndex + 1, end);
        };
        
        quickSortHelper(0, newArray.length - 1);
        break;
      }
      case 'mergeSort': {
        const mergeSortHelper = (start, end) => {
          if (start >= end) return;
          
          const mid = Math.floor((start + end) / 2);
          mergeSortHelper(start, mid);
          mergeSortHelper(mid + 1, end);
          
          // Merge process
          const leftArr = newArray.slice(start, mid + 1);
          const rightArr = newArray.slice(mid + 1, end + 1);
          
          let i = 0, j = 0, k = start;
          
          while (i < leftArr.length && j < rightArr.length) {
            // Compare elements from temp arrays
            animations.push({ type: 'compare', indices: [start + i, mid + 1 + j] });
            comparisonsCount++;
            
            if (leftArr[i] <= rightArr[j]) {
              newArray[k] = leftArr[i];
              animations.push({ type: 'overwrite', index: k, value: leftArr[i] });
              i++;
            } else {
              newArray[k] = rightArr[j];
              animations.push({ type: 'overwrite', index: k, value: rightArr[j] });
              j++;
            }
            k++;
          }
          
          while (i < leftArr.length) {
            newArray[k] = leftArr[i];
            animations.push({ type: 'overwrite', index: k, value: leftArr[i] });
            i++;
            k++;
          }
          
          while (j < rightArr.length) {
            newArray[k] = rightArr[j];
            animations.push({ type: 'overwrite', index: k, value: rightArr[j] });
            j++;
            k++;
          }
          
          // Mark the merged segment as sorted
          for (let idx = start; idx <= end; idx++) {
            markSorted(idx);
          }
        };
        
        mergeSortHelper(0, newArray.length - 1);
        break;
      }
      case 'heapSort': {
        const heapify = (n, i) => {
          let largest = i;
          const left = 2 * i + 1;
          const right = 2 * i + 2;
          
          if (left < n && compare(left, largest) > 0) {
            largest = left;
          }
          
          if (right < n && compare(right, largest) > 0) {
            largest = right;
          }
          
          if (largest !== i) {
            swap(i, largest);
            heapify(n, largest);
          }
        };
        
        // Build heap
        for (let i = Math.floor(newArray.length / 2) - 1; i >= 0; i--) {
          heapify(newArray.length, i);
        }
        
        // Extract elements from heap
        for (let i = newArray.length - 1; i > 0; i--) {
          swap(0, i);
          markSorted(i);
          heapify(i, 0);
        }
        markSorted(0);
        break;
      }
    }

    setTotalSteps(animations.length);
    setComparisons(comparisonsCount);
    setSwaps(swapsCount);
    setAnimationQueue(animations);
    animationQueueRef.current = animations;
    return animations;
  };

  // Start sorting animation
  const startSorting = () => {
    if (isSorting) {
      if (isPlayingRef.current) {
        // Pause
        isPlayingRef.current = false;
      } else {
        // Resume
        isPlayingRef.current = true;
        runAnimation();
      }
    } else {
      // Start new sorting
      const animations = processAlgorithm(sortingAlgorithm);
      if (animations.length > 0) {
        setIsSorting(true);
        isPlayingRef.current = true;
        runAnimation();
      }
    }
  };

  // Step through sorting
  const stepSorting = () => {
    if (!isSorting) {
      const animations = processAlgorithm(sortingAlgorithm);
      if (animations.length > 0) {
        setIsSorting(true);
        isPlayingRef.current = false;
        executeAnimationStep();
      }
    } else {
      if (!isPlayingRef.current) {
        executeAnimationStep();
      }
    }
  };

  // Execute a single animation step
  const executeAnimationStep = () => {
    if (currentStep < animationQueueRef.current.length) {
      const animation = animationQueueRef.current[currentStep];
      
      if (animation.type === 'compare') {
        setCurrentIndices(animation.indices);
      } else if (animation.type === 'swap') {
        setCurrentIndices(animation.indices);
        setArray(prev => {
          const newArray = [...prev];
          const [i, j] = animation.indices;
          [newArray[i], newArray[j]] = [newArray[j], newArray[i]];
          return newArray;
        });
      } else if (animation.type === 'sorted') {
        setSortedIndices(prev => [...prev, animation.index]);
      } else if (animation.type === 'overwrite') {
        setArray(prev => {
          const newArray = [...prev];
          newArray[animation.index] = animation.value;
          return newArray;
        });
      }
      
      setCurrentStep(prev => prev + 1);
    } else {
      // Animation complete
      setCurrentIndices([]);
      isPlayingRef.current = false;
      if (requestRef.current) {
        cancelAnimationFrame(requestRef.current);
        requestRef.current = null;
      }
    }
  };

  // Run animation loop
  const runAnimation = () => {
    if (!isPlayingRef.current) return;
    
    executeAnimationStep();
    
    const timeout = 1000 / speed;
    requestRef.current = setTimeout(() => {
      if (currentStep < animationQueueRef.current.length && isPlayingRef.current) {
        runAnimation();
      } else {
        isPlayingRef.current = false;
      }
    }, timeout);
  };

  // Clean up on unmount
  useEffect(() => {
    return () => {
      if (requestRef.current) {
        clearTimeout(requestRef.current);
      }
    };
  }, []);
  
  // Update refs when state changes
  useEffect(() => {
    animationQueueRef.current = animationQueue;
  }, [animationQueue]);

  // Algorithm explanations
  const algorithmInfo = {
    bubbleSort: {
      title: "Bubble Sort",
      description: "Repeatedly steps through the list, compares adjacent elements and swaps them if they are in the wrong order.",
      timeComplexity: "O(n²)",
      spaceComplexity: "O(1)"
    },
    selectionSort: {
      title: "Selection Sort",
      description: "Divides input into sorted and unsorted regions, repeatedly selects smallest element from unsorted region.",
      timeComplexity: "O(n²)",
      spaceComplexity: "O(1)"
    },
    insertionSort: {
      title: "Insertion Sort",
      description: "Builds sorted array one item at a time, taking each element and inserting it into its correct position.",
      timeComplexity: "O(n²)",
      spaceComplexity: "O(1)"
    },
    quickSort: {
      title: "Quick Sort",
      description: "Divides array into smaller subarrays using a pivot, recursively sorts subarrays.",
      timeComplexity: "O(n log n) average, O(n²) worst",
      spaceComplexity: "O(log n)"
    },
    mergeSort: {
      title: "Merge Sort",
      description: "Divides array into halves, sorts them recursively, then merges sorted halves.",
      timeComplexity: "O(n log n)",
      spaceComplexity: "O(n)"
    },
    heapSort: {
      title: "Heap Sort",
      description: "Converts array into a heap data structure, repeatedly extracts maximum element.",
      timeComplexity: "O(n log n)",
      spaceComplexity: "O(1)"
    }
  };

  const currentAlgoInfo = algorithmInfo[sortingAlgorithm];

  return (
    <div className="flex flex-col min-h-screen bg-gray-100 p-4 md:p-8">
      <div className="text-center mb-8">
        <h1 className="text-3xl font-bold text-gray-800">Algorithm Visualizer</h1>
        <p className="text-gray-600">Visualize sorting algorithms in action</p>
      </div>

      <div className="bg-white rounded-lg shadow-md p-4 mb-6">
        <div className="flex flex-wrap gap-4 mb-4">
          <div className="flex-1">
            <label className="block text-gray-700 mb-1">Algorithm</label>
            <select
              className="w-full p-2 border rounded bg-white"
              value={sortingAlgorithm}
              onChange={handleAlgorithmChange}
              disabled={isSorting && isPlayingRef.current}
            >
              <option value="bubbleSort">Bubble Sort</option>
              <option value="selectionSort">Selection Sort</option>
              <option value="insertionSort">Insertion Sort</option>
              <option value="quickSort">Quick Sort</option>
              <option value="mergeSort">Merge Sort</option>
              <option value="heapSort">Heap Sort</option>
            </select>
          </div>
          
          <div className="flex-1">
            <label className="block text-gray-700 mb-1">
              Array Size: {arraySize}
            </label>
            <input
              type="range"
              min="5"
              max="100"
              value={arraySize}
              onChange={handleSizeChange}
              disabled={isSorting && isPlayingRef.current}
              className="w-full"
            />
          </div>
          
          <div className="flex-1">
            <label className="block text-gray-700 mb-1">
              Speed: {speed}
            </label>
            <input
              type="range"
              min="1"
              max="100"
              value={speed}
              onChange={handleSpeedChange}
              className="w-full"
            />
          </div>
        </div>

        <div className="flex flex-wrap gap-2">
          <button
            onClick={startSorting}
            className={`px-4 py-2 rounded ${
              isSorting && isPlayingRef.current
                ? "bg-yellow-500 hover:bg-yellow-600"
                : "bg-green-500 hover:bg-green-600"
            } text-white font-medium`}
          >
            {isSorting ? (isPlayingRef.current ? "Pause" : "Resume") : "Start"}
          </button>
          
          <button
            onClick={stepSorting}
            disabled={isSorting && isPlayingRef.current}
            className={`px-4 py-2 rounded bg-blue-500 hover:bg-blue-600 text-white font-medium ${
              isSorting && isPlayingRef.current ? "opacity-50 cursor-not-allowed" : ""
            }`}
          >
            Step
          </button>
          
          <button
            onClick={resetArray}
            className="px-4 py-2 rounded bg-red-500 hover:bg-red-600 text-white font-medium"
            disabled={isSorting && isPlayingRef.current}
          >
            Reset
          </button>
        </div>
      </div>

      <div className="bg-white rounded-lg shadow-md p-4 mb-6 flex-grow">
        <div className="flex justify-between mb-2">
          <div className="text-sm text-gray-600">
            Step: {currentStep}/{totalSteps}
          </div>
          <div className="text-sm text-gray-600">
            Comparisons: {comparisons} | Swaps: {swaps}
          </div>
        </div>

        <div className="h-64 flex items-end justify-center border-b border-gray-200">
          {array.map((value, idx) => (
            <div
              key={idx}
              className={`mx-0.5 w-4 ${
                currentIndices.includes(idx)
                  ? "bg-yellow-500"
                  : sortedIndices.includes(idx)
                  ? "bg-green-500"
                  : "bg-blue-500"
              } transition-all duration-200`}
              style={{ height: `${value * 2}px` }}
            ></div>
          ))}
        </div>
      </div>

      <div className="bg-white rounded-lg shadow-md p-4">
        <h2 className="text-xl font-bold text-gray-800 mb-2">{currentAlgoInfo.title}</h2>
        <p className="text-gray-600 mb-2">{currentAlgoInfo.description}</p>
        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
          <div>
            <h3 className="font-semibold text-gray-700">Time Complexity</h3>
            <p className="text-gray-600">{currentAlgoInfo.timeComplexity}</p>
          </div>
          <div>
            <h3 className="font-semibold text-gray-700">Space Complexity</h3>
            <p className="text-gray-600">{currentAlgoInfo.spaceComplexity}</p>
          </div>
        </div>
      </div>
    </div>
  );
}