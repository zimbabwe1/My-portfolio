// DOM Elements
const menuToggle = document.querySelector('.menu-toggle');
const navLinks = document.querySelector('.nav-links');
const navItems = document.querySelectorAll('.nav-links a');
const contactForm = document.querySelector('.contact-form');
const statNumbers = document.querySelectorAll('.stat-number');

// Mobile Menu Toggle
if (menuToggle) {
  menuToggle.addEventListener('click', () => {
    navLinks.classList.toggle('active');
  });
}

// Close menu when nav item clicked
navItems.forEach(item => {
  item.addEventListener('click', () => {
    navLinks.classList.remove('active');
  });
});

// Active nav link on scroll
const highlightNavItem = () => {
  const sections = document.querySelectorAll('section[id]');
  const scrollPosition = window.scrollY + 100;

  sections.forEach(section => {
    const sectionTop = section.offsetTop;
    const sectionHeight = section.offsetHeight;
    const sectionId = section.getAttribute('id');
    const navItem = document.querySelector(`.nav-links a[href="#${sectionId}"]`);

    if (scrollPosition >= sectionTop && scrollPosition < sectionTop + sectionHeight) {
      navItems.forEach(item => item.classList.remove('active'));
      if (navItem) navItem.classList.add('active');
    }
  });
};

window.addEventListener('scroll', highlightNavItem);

// Counter Animation
const animateCounters = () => {
  statNumbers.forEach(stat => {
    const target = parseInt(stat.getAttribute('data-target'));
    const duration = 1000;
    const increment = target / (duration / 16);
    let current = 0;

    const updateCounter = () => {
      current += increment;
      if (current < target) {
        stat.textContent = Math.floor(current);
        requestAnimationFrame(updateCounter);
      } else {
        stat.textContent = target;
      }
    };

    // Trigger animation when element is visible
    const observer = new IntersectionObserver(entries => {
      entries.forEach(entry => {
        if (entry.isIntersecting) {
          updateCounter();
          observer.unobserve(entry.target);
        }
      });
    });

    observer.observe(stat);
  });
};

animatateCounters();

// Form Validation and Submission
if (contactForm) {
  contactForm.addEventListener('submit', (e) => {
    e.preventDefault();

    const formData = new FormData(contactForm);
    const name = formData.get('name') || contactForm.querySelector('input[placeholder="Your name"]').value;
    const email = formData.get('email') || contactForm.querySelector('input[placeholder="you@example.com"]').value;
    const message = formData.get('message') || contactForm.querySelector('textarea').value;

    // Basic validation
    if (!name.trim()) {
      alert('Please enter your name');
      return;
    }

    if (!email.trim() || !isValidEmail(email)) {
      alert('Please enter a valid email');
      return;
    }

    if (!message.trim()) {
      alert('Please enter a message');
      return;
    }

    // Success message
    alert(`Thanks ${name}! I'll get back to you soon.`);
    contactForm.reset();
  });
}

// Email validation
function isValidEmail(email) {
  const regex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
  return regex.test(email);
}

// Intersection Observer for reveal animations
const observerOptions = {
  threshold: 0.1,
  rootMargin: '0px 0px -50px 0px'
};

const observer = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      entry.target.style.animationDelay = '0s';
    }
  });
}, observerOptions);

document.querySelectorAll('.reveal').forEach(el => {
  observer.observe(el);
});

// Smooth scroll for internal links
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
  anchor.addEventListener('click', (e) => {
    e.preventDefault();
    const target = document.querySelector(anchor.getAttribute('href'));
    if (target) {
      target.scrollIntoView({ behavior: 'smooth', block: 'start' });
    }
  });
});

// Initialize
window.addEventListener('load', () => {
  highlightNavItem();
});
