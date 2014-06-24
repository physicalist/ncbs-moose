-- This haskell script compares data generated by NEURON and MOOSE. This is not
-- a generic script.
import System.Environment
import qualified Data.Text.IO as IO
import qualified Data.Text as T

txtToData text = map toFloat lines where
    lines = T.lines text

main = do
    args <- getArgs  
    a <- mapM IO.readFile args
    let r = map txtToData a
    print r
    putStrLn "Done"
