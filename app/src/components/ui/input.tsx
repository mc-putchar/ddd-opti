import * as React from "react"

import { cn } from "@/lib/utils"

export interface InputProps
  extends React.InputHTMLAttributes<HTMLInputElement> {}

const Input = React.forwardRef<HTMLInputElement, InputProps>(
  ({ className, type, ...props }, ref) => {
    return (
      <input
        type={type}
        className={cn(
          "flex h-9 w-full border border-input bg-transparent px-3 py-0 text-sm shadow-sm transition-colors file:border-0 file:bg-transparent file:text-sm file:font-medium file:text-foreground placeholder:text-muted-foreground focus-visible:outline-none focus-visible:ring-1 focus-visible:ring-ring disabled:cursor-not-allowed disabled:opacity-50",
          className
        )}
        ref={ref}
        {...props}
		style={{
			WebkitAppearance: 'none', // Hide the spinner in Webkit browsers
			MozAppearance: 'textfield', // Hide the spinner in Firefox
			appearance: 'none', // General appearance property to remove spinners
		  }}
      />
    )
  }
)
Input.displayName = "Input"

export { Input }
