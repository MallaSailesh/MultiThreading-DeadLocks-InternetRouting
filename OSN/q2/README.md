**Q2**

* Question) The pick-up spot now has a stipulated amount of pizzas it can hold. If the
pizzas are full, chefs route the pizzas to a secondary storage. How would you
handle such a situation? 
* Ans) i will intialise a semaphore with value equal to that of max number of pizzas it hold . If the value of semaphore is zero then if any pizza comes to the pick-up spot then chef routes it into secondary storage and after waiting is over (i.e there is space to keep the pizza) then we move the pizza back to the pick-up spot form the secondary storage . Here we will first move the pizza to the pick-up spot which was the first one to arrive after the pick-up spot is filled with max capacity .

* Question) Each incomplete order affects the ratings of the restaurant. Given the past
histories of orders, how would you re-design your simulation to have lesser
incomplete orders? Note that the rating of the restaurant is not affected if the
order is rejected instantaneously on arrival.
* Ans) Allocate the ingredients to the order(pizzas) which uses less ingredients first then remaining ones . so we will sort the pizzas with the number of ingredients it uses . then we start allocating ingredients to pizzas form the starting and thus we reject the customers who ordered the pizzas which were not allocated ingredients in the allocation pizzas done above . 

* Question)Ingredients can be replenished on calling the nearest supermarket. How
would your drive-thru rejection / acceptance change based on this?
* Ans) Here in this condition when we knew in the ingredients isn't sufficient then we will replenish from  near super market , so that now ingredients will now increase . Thus from now the algorithm is work same 

**Algorithm and implementation details :-**

* A thread for each chef and for each customer . thread for each chef ensures the arrival time and exit time and by that it will check who are all the the chefs working . for this working variable is there in the struct .
* In struct customer we store a variable called check which will store for how many pizzas he ordered the ingredients are sufficient , cnt to keep track of number of pizzas ordered by that customer , pizzas array store the id of the pizzas and pizza_st stores the value zero if the ingredients aren't sufficient else it stores the value x(xth pizza ordered on that day) which ensures which pizza thread are we dealing with (thread is created for the pizza iff ingredients sufficiently are there of it). This part is done in a customer thread . for each customer thread i created multiple threads each one for 1 pizza because each pizza independetly checks for the chef who is free in the order of chefs . 
* Now chefs check for oven allocation in the order (i.e first check for oven 1 then oven 2 so on ).
* In above two cases i.e chef allocation for pizza and oven allocation for pizza , i used sem_timedwait so that it will check for the avaliabiltiy of chefs or ovens if not available it will wait for 1 second and again checks if they are free and so on .  The cusotmer is rejected if there are no chefs working in the pizzateria checked by checking the working status of chefs which is -1 for all then customer is rejected . 
* sem_timedwait is used in above two cases because if more than 1 pizza is waiting for chef , then as soon as chef is ready to make pizza both pizzas gets allocated to that chef which is wrong , so used semaphore so that only for 1 pizza that chef is assigned . similarly for ovens this is done 
