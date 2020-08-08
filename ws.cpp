#include "ws_parse.h"
#include "ws_exec.h"

template<typename CharT, CharT... C> 
constexpr auto operator"" _ws()
{
  using tokens = tokenize<C...>;
  return types_apply_t<parse_t,tokens>{};
}

int main () {
  R"(Say hello.   
   	  	   
		    	
   		  	 	
		    	 
   		 		  
		    		
   		 		  
		    	  
   		 				
		    	 	
   	 		  
		    		 
   	     
		    			
   			 			
		    	   
   		 				
		    	  	
   			  	 
		    	 	 
   		 		  
		    	 		
   		  	  
		    		  
   	     
		    		 	
   		 				
		    			 
   		  		 
		    				
   	     
		    	    
   			  		
		    	   	
   			    
		    	  	 
   		    	
		    	  		
   		   		
		    	 	  
   		  	 	
		    	 	 	
   			  		
		    	 		 
   	    	
		    	 			
    
		     

 	 			 			 			  	  		 	  	 			 	   		  	 	

 	 		 			  		  	 	 			 			 		 		   		 	  	 		 			  		  	 	




   		    	 		  	   		  	  
	   
	

   			 			 			  	  		 	  	 			 	   		  	 	
 
 			 
 
	  			 			 			  	  		 	  	 			 	   		  	 	 	 					 		  	 	 		 			  		  	  
	
     	
	   
 
 			 			 			  	  		 	  	 			 	   		  	 	

   			 			 			  	  		 	  	 			 	   		  	 	 	 					 		  	 	 		 			  		  	  
 

 


	

   			  	  		  	 	 		    	 		  	  
 
  
 	
	 			 
    	 	 
	  	
	  			  	  		  	 	 		    	 		  	   	 					 		  	 	 		 			  		  	  
 

   	
	   
 
 			  	  		  	 	 		    	 		  	  

   			  	  		  	 	 		    	 		  	   	 					 		  	 	 		 			  		  	  
 

   	
	       
		 
	

   		 			  		  	 	 			 			 		 		   		 	  	 		 			  		  	 	
   	 	 
   		 	
	
  	
  
	
)"_ws.run();
  return 0;
}